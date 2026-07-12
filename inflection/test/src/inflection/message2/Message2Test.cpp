/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/exception/IOException.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/PronounConcept.hpp>
#include <inflection/dialog/SemanticConcept.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/SemanticValue.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/message2/MF2Factory.hpp>
#include <inflection/message2/SemanticFeatureConceptArgument.hpp>
#include <inflection/npc.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringUtils.hpp>

#include "util/TestUtils.hpp"
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "unicode/locid.h"
#include "unicode/messageformat2.h"
#include "unicode/messageformat2_function_registry.h"
#include "unicode/messageformat2_formattable.h"

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <vector>

using icu::Locale;
using icu::UnicodeString;
using icu::message2::Formattable;
using icu::message2::data_model::FunctionName;
using icu::message2::MFFunctionRegistry;
using icu::message2::MessageArguments;
using icu::message2::MessageFormatter;

static void
validateNodeName(const xmlNodePtr node, const char *name)
{
    if (xmlStrEqual(node->name, (const xmlChar *) name) == 0) {
        throw ::inflection::exception::XMLParseException(u"Expecting element <" + ::inflection::util::StringUtils::to_u16string(name) + u">, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(node->name))) + u">");
    }
}

static std::u16string
getRequiredAttribute(const xmlNodePtr node, const char *attributeName)
{
    xmlChar * attributeValue = xmlGetProp(node, (const xmlChar *)attributeName);
    if (attributeValue == nullptr) {
        throw ::inflection::exception::XMLParseException(std::u16string(u"The attribute ") + ::inflection::util::StringUtils::to_u16string(attributeName) + u" is required on " + ::inflection::util::StringUtils::to_u16string((const char *)npc(node)->name));
    }
    ::std::u16string attributeStr;
    ::inflection::util::StringUtils::convert(&attributeStr, (const char *)attributeValue);
    xmlFree(attributeValue);
    return attributeStr;
}

static void
getContent(::std::u16string* result, const xmlNodePtr nodePtr)
{
    xmlChar* content = xmlNodeGetContent(nodePtr);
    ::inflection::util::StringUtils::convert(result, (const char*)content);
    xmlFree(content);
}

static std::map<inflection::dialog::SemanticFeature, std::u16string>
getConstraints(const inflection::dialog::SemanticFeatureModel &model, const ::std::set<::std::u16string>& knownCategoryValues, xmlNode *node, const char* propertyName)
{
    std::map<inflection::dialog::SemanticFeature, std::u16string> result;
    xmlChar * varInflect = xmlGetProp(node, (const xmlChar *)propertyName);
    if (varInflect != nullptr) {
        ::std::u16string inflectParams;
        inflection::util::StringUtils::convert(&inflectParams, (const char *) varInflect);
        for (inflection::util::DelimitedStringIterator iterator(inflectParams, u" "); iterator.hasNext(); ++iterator) {
            ::std::u16string constraint(*iterator);
            ::std::u16string constraintName;
            auto equalsIdx = constraint.find(u'=');
            if (equalsIdx != std::u16string::npos) {
                constraintName.assign(constraint, 0, equalsIdx);
                constraint.erase(0, equalsIdx + 1);
            }
            else {
                constraintName = constraint;
            }
            auto semanticFeaturePtr = model.getFeature(constraintName);
            if (semanticFeaturePtr == nullptr) {
                auto semanticFeaturePair = model.getFeatureAlias(constraintName);
                if (semanticFeaturePair == ::std::pair<inflection::dialog::SemanticFeature *, ::std::u16string>()) {
                    throw inflection::exception::XMLParseException(u"Constraint is not known: " + inflectParams);
                }
                semanticFeaturePtr = semanticFeaturePair.first;
            }
            if (!knownCategoryValues.contains(constraint)) {
                result.emplace(*npc(semanticFeaturePtr), std::u16string());
            }
            else {
                result.emplace(*npc(semanticFeaturePtr), constraint);
            }
        }
    }
    xmlFree(varInflect);
    return result;
}

static ::inflection::dialog::SemanticFeatureModel_DisplayData
getDisplayData(const inflection::dialog::SemanticFeatureModel &model, const ::std::set<::std::u16string>& knownCategoryValues, xmlNodePtr nodePtr)
{
    std::vector<inflection::dialog::DisplayValue> displayValues;
    ::std::u16string value;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode, "value");
        auto constraints = getConstraints(model, knownCategoryValues, valueNode, "constraint");
        getContent(&value, valueNode);
        displayValues.emplace_back(value, constraints);
    }
    return ::inflection::dialog::SemanticFeatureModel_DisplayData(displayValues);
}

// Collects the display data for every type="SemanticConcept" param across the whole file,
// keyed by SemanticValue(u"", paramName). The result builds a single per-file
// SemanticFeatureModel, so the expensive model is created once instead of per object.
static std::map<inflection::dialog::SemanticValue, inflection::dialog::SemanticFeatureModel_DisplayData>
collectSemanticValues(const inflection::dialog::SemanticFeatureModel& model,
                      const std::set<std::u16string>& knownCategoryValues, xmlNodePtr root)
{
    std::map<inflection::dialog::SemanticValue, inflection::dialog::SemanticFeatureModel_DisplayData> result;
    for (xmlNodePtr testNode = root->children; testNode != nullptr; testNode = testNode->next) {
        if (testNode->type != XML_ELEMENT_NODE || xmlStrEqual(testNode->name, (const xmlChar *) "test") == 0) {
            continue;
        }
        for (xmlNodePtr prop = testNode->children; prop != nullptr; prop = prop->next) {
            if (prop->type != XML_ELEMENT_NODE || xmlStrEqual(prop->name, (const xmlChar *) "params") == 0) {
                continue;
            }
            for (xmlNodePtr paramNode = prop->children; paramNode != nullptr; paramNode = paramNode->next) {
                if (paramNode->type != XML_ELEMENT_NODE) {
                    continue;
                }
                validateNodeName(paramNode, "param");
                if (getRequiredAttribute(paramNode, "type") != u"SemanticConcept") {
                    continue;
                }
                std::u16string name = getRequiredAttribute(paramNode, "name");
                result.emplace(inflection::dialog::SemanticValue(u"", name),
                               getDisplayData(model, knownCategoryValues, paramNode));
            }
        }
    }
    return result;
}

static void
addParam(std::map<UnicodeString, Formattable>& arguments, xmlNodePtr paramNode,
         std::vector<std::unique_ptr<Formattable[]>>& arrayStorage,
         const inflection::dialog::SemanticFeatureModel& model,
         const std::set<std::u16string>& knownCategoryValues,
         std::vector<std::unique_ptr<icu::message2::FormattableObject>>& objectStorage)
{
    std::u16string name = getRequiredAttribute(paramNode, "name");
    std::u16string type = getRequiredAttribute(paramNode, "type");
    std::vector<std::u16string> values;
    for (xmlNodePtr child = paramNode->children; child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(child, "value");
        std::u16string value;
        getContent(&value, child);
        values.push_back(value);
    }
    if (type == u"String") {
        arguments[UnicodeString(name.c_str())] = Formattable(UnicodeString(values.at(0).c_str()));
    }
    else if (type == u"Number") {
        std::string narrow = inflection::util::StringUtils::to_string(values.at(0));
        try {
            if (narrow.find('.') == std::string::npos && narrow.find('e') == std::string::npos
                && narrow.find('E') == std::string::npos) {
                arguments[UnicodeString(name.c_str())] = Formattable((int64_t) std::stoll(narrow));
            } else {
                arguments[UnicodeString(name.c_str())] = Formattable(std::stod(narrow));
            }
        } catch (const std::exception&) {
            throw ::inflection::exception::XMLParseException(u"Invalid Number value: " + values.at(0));
        }
    }
    else if (type == u"String[]") {
        int32_t n = (int32_t) values.size();
        auto arr = std::make_unique<Formattable[]>(n);
        for (int32_t i = 0; i < n; i++) {
            arr[i] = Formattable(UnicodeString(values[(size_t)i].c_str()));
        }
        arguments[UnicodeString(name.c_str())] = Formattable(arr.get(), n);
        arrayStorage.push_back(std::move(arr));
    }
    else if (type == u"SemanticConcept") {
        // model here is the per-file svMap model (a superset of the base grammar model) that
        // collectSemanticValues populated, so the display data is already registered.
        auto sc = std::make_unique<inflection::dialog::SemanticConcept>(
            &model, inflection::dialog::SemanticValue(u"", name));
        auto obj = std::make_unique<inflection::message2::SemanticFeatureConceptArgument>(sc.release());
        arguments[UnicodeString(name.c_str())] = Formattable(obj.get());   // not adopted
        objectStorage.push_back(std::move(obj));
    }
    else if (type == u"PronounConcept") {
        std::vector<inflection::dialog::DisplayValue> displayValues =
            getDisplayData(model, knownCategoryValues, paramNode).getValues();
        std::unique_ptr<inflection::dialog::PronounConcept> pronoun;
        if (displayValues.size() == 1 && displayValues[0].getConstraintMap().empty()) {
            // A single bare <value> is a pronoun seed, parsed like a String operand.
            pronoun = std::make_unique<inflection::dialog::PronounConcept>(
                model, std::u16string_view(displayValues[0].getDisplayString()));
        } else {
            // Rows carrying constraints define a custom pronoun (e.g. neopronouns).
            pronoun = std::make_unique<inflection::dialog::PronounConcept>(
                model, displayValues, std::map<inflection::dialog::SemanticFeature, std::u16string>());
        }
        auto obj = std::make_unique<inflection::message2::SemanticFeatureConceptArgument>(pronoun.release());
        arguments[UnicodeString(name.c_str())] = Formattable(obj.get());   // not adopted
        objectStorage.push_back(std::move(obj));
    }
    else {
        throw ::inflection::exception::XMLParseException(u"Type is not recognized (supported: String, Number, String[], SemanticConcept, PronounConcept): " + type);
    }
}

static void
deriveFromSSML(const std::u16string& ssml, std::u16string* print, std::u16string* speak)
{
    std::string utf8 = inflection::util::StringUtils::to_string(u"<root>" + ssml + u"</root>");
    xmlDocPtr doc = xmlParseMemory(utf8.c_str(), (int)utf8.size());
    if (doc == nullptr) {
        throw ::inflection::exception::XMLParseException(u"Bad SSML: " + ssml);
    }
    xmlNodePtr root = xmlDocGetRootElement(doc);
    try {
        for (xmlNodePtr n = root->children; n != nullptr; n = n->next) {
            if (n->type == XML_TEXT_NODE) {
                std::u16string t;
                getContent(&t, n);
                *print += t;
                *speak += t;
            } else if (n->type == XML_ELEMENT_NODE && xmlStrEqual(n->name, (const xmlChar*)"sub")) {
                std::u16string content;
                getContent(&content, n);
                *print += content;
                *speak += getRequiredAttribute(n, "alias");
            }
        }
    } catch (...) {
        xmlFreeDoc(doc);
        throw;
    }
    xmlFreeDoc(doc);
}

TEST_CASE("Message2Test#testLocales")
{
    xmlLineNumbersDefault(1);
    UErrorCode errorCode = U_ZERO_ERROR;

    // Build one registry per output mode (subsumes MF2FactoryTest#testBasic).
    auto buildRegistry = [&](inflection::message2::MF2Factory::OutputMode mode) {
        UErrorCode ec = U_ZERO_ERROR;
        auto r = MFFunctionRegistry::Builder(ec)
            .adoptFormatter(FunctionName("inflect"), inflection::message2::MF2Factory::createInflectFormatterFactory(mode), ec)
            .adoptSelector(FunctionName("inflect"), inflection::message2::MF2Factory::createInflectSelectorFactory(), ec)
            .adoptFormatter(FunctionName("numeral"), inflection::message2::MF2Factory::createNumeralFormatterFactory(mode), ec)
            .adoptFormatter(FunctionName("quantify"), inflection::message2::MF2Factory::createQuantifyFormatterFactory(mode), ec)
            .adoptFormatter(FunctionName("list"), inflection::message2::MF2Factory::createListFormatterFactory(mode), ec)
            .adoptFormatter(FunctionName("pronoun"), inflection::message2::MF2Factory::createPronounFormatterFactory(mode), ec)
            .adoptSelector(FunctionName("pronoun"), inflection::message2::MF2Factory::createPronounSelectorFactory(), ec)
            .build();
        REQUIRE(U_SUCCESS(ec));
        return r;
    };
    auto plainRegistry = buildRegistry(inflection::message2::MF2Factory::OutputMode::PLAIN);
    auto ssmlRegistry  = buildRegistry(inflection::message2::MF2Factory::OutputMode::SSML);
    REQUIRE(U_SUCCESS(errorCode));

    const std::string message2Dir = TestUtils::getTestResourcePath() + std::string("message2/");

    // Supported locales are region-coded (e.g. de_DE/de_AT/de_CH), so each test file must be
    // named after one. Build the set once to validate every file we enumerate below; a file
    // naming an unsupported locale would otherwise be skipped silently and pass vacuously.
    std::set<std::string> supportedLocaleNames;
    for (const auto& supportedLocale : ::inflection::util::LocaleUtils::getSupportedLocaleList()) {
        supportedLocaleNames.insert(supportedLocale.getName());
    }

    // Drive the test off the files actually present rather than probing every supported locale.
    const std::string xmlSuffix(".xml");
    auto files(TestUtils::listDirectoryContents(message2Dir));
    std::sort(files.begin(), files.end());
    int32_t executedFiles = 0;
    for (const auto& file : files) {
        if (file.length() < xmlSuffix.length()
            || file.compare(file.length() - xmlSuffix.length(), xmlSuffix.length(), xmlSuffix) != 0) {
            continue; // Ignore non-XML entries (e.g. the .xsd schema).
        }
        std::string localeName(file.substr(0, file.length() - xmlSuffix.length()));
        const bool isSupportedLocale = supportedLocaleNames.contains(localeName);
        INFO(std::string("Test file message2/") + file + std::string(" names locale '") + localeName
            + std::string("', which is not in the supported locale list (region-coded, e.g. de_DE)."));
        CHECK(isSupportedLocale);
        if (!isSupportedLocale) {
            continue; // Don't try to build a model for an unsupported locale.
        }

        ::inflection::util::ULocale locale(localeName);
        std::string resourcePath = message2Dir + file;
        ::std::u16string resourceFile;
        inflection::util::StringUtils::convert(&resourceFile, resourcePath);
        ::inflection::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceFile);
        }
        xmlNodePtr root = xmlDocGetRootElement(xmlDoc);
        if (root == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <message2test>, got <null>");
        }
        validateNodeName(root, "message2test");

        Locale icuLocale(locale.getName().c_str());

        auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
        ::std::set<::std::u16string> knownCategoryValues;
        for (const auto& grammarCategory : features.getCategories()) {
            for (const auto& value : grammarCategory.second.getValues()) {
                knownCategoryValues.insert(value);
            }
        }
        inflection::dialog::SemanticFeatureModel model(locale);
        // Build one SemanticFeatureModel carrying every SemanticConcept's display data for
        // the whole file (only when the file actually uses them). SemanticConcept requires a
        // model constructed with this map, and the model is expensive to build, so it is made
        // once here rather than once per object.
        auto semanticValueMap = collectSemanticValues(model, knownCategoryValues, root);
        std::unique_ptr<inflection::dialog::SemanticFeatureModel> semanticModel;
        if (!semanticValueMap.empty()) {
            semanticModel = std::make_unique<inflection::dialog::SemanticFeatureModel>(locale, semanticValueMap);
        }

        for (xmlNodePtr testNode = root->children; testNode != nullptr; testNode = testNode->next) {
            if (testNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            validateNodeName(testNode, "test");

            std::map<UnicodeString, Formattable> arguments;
            std::vector<std::unique_ptr<Formattable[]>> arrayStorage;
            std::vector<std::unique_ptr<icu::message2::FormattableObject>> objectStorage;
            std::u16string source;
            std::u16string print;
            std::u16string speakExpected;
            bool hasSpeak = false;
            bool errorExpected = false;
            std::u16string errorName;

            for (xmlNodePtr prop = testNode->children; prop != nullptr; prop = prop->next) {
                if (prop->type != XML_ELEMENT_NODE) {
                    continue;
                }
                if (xmlStrEqual(prop->name, (const xmlChar *) "params")) {
                    for (xmlNodePtr p = prop->children; p != nullptr; p = p->next) {
                        if (p->type != XML_ELEMENT_NODE) {
                            continue;
                        }
                        validateNodeName(p, "param");
                        // Use the svMap model for object creation when the file needs it; it is a
                        // superset of the base grammar model, so it also serves String/pronoun params.
                        const auto& objectModel = semanticModel ? *semanticModel : model;
                        addParam(arguments, p, arrayStorage, objectModel, knownCategoryValues, objectStorage);
                    }
                }
                else if (xmlStrEqual(prop->name, (const xmlChar *) "source")) {
                    getContent(&source, prop);
                }
                else if (xmlStrEqual(prop->name, (const xmlChar *) "print")) {
                    getContent(&print, prop);
                    xmlChar* errAttr = xmlGetProp(prop, (const xmlChar *)"error");
                    if (errAttr != nullptr) {
                        errorExpected = true;
                        inflection::util::StringUtils::convert(&errorName, (const char*)errAttr);
                        xmlFree(errAttr);
                    }
                }
                else if (xmlStrEqual(prop->name, (const xmlChar *) "speak")) {
                    getContent(&speakExpected, prop);
                    hasSpeak = true;
                }
                else {
                    FAIL(std::string("Unknown property ") + std::string((const char *)prop->name));
                }
            }

            UErrorCode testError = U_ZERO_ERROR;
            UParseError pe;
            MessageFormatter::Builder builder(testError);
            builder.setFunctionRegistry(hasSpeak ? ssmlRegistry : plainRegistry)
                .setLocale(icuLocale)
                .setPattern(UnicodeString(source.c_str()), pe, testError);
            if (errorExpected) {
                // Surface MF errors (e.g. an unknown :inflect feature) via the UErrorCode.
                builder.setErrorHandlingBehavior(MessageFormatter::U_MF_STRICT);
            }
            MessageFormatter mf = builder.build(testError);
            INFO(std::string("Locale: ") + locale.getName());
            INFO(std::string("Source: ") + inflection::util::StringUtils::to_string(source));
            INFO(std::string("XML line: ") + std::to_string(xmlGetLineNo(testNode)));

            if (errorExpected) {
                // The error may surface at build() or formatToString() (strict mode).
                if (U_SUCCESS(testError)) {
                    mf.formatToString(MessageArguments(arguments, testError), testError);
                }
                std::string gotError = u_errorName(testError);
                INFO(std::string("Got error: ") + gotError);
                INFO(std::string("Exp error: ") + inflection::util::StringUtils::to_string(errorName));
                CHECK(U_FAILURE(testError));
                CHECK(gotError == inflection::util::StringUtils::to_string(errorName));
            } else {
                INFO(std::string("Parse error at line ") + std::to_string(pe.line) + std::string(", offset ") + std::to_string(pe.offset));
                REQUIRE(U_SUCCESS(testError));

                UnicodeString result = mf.formatToString(MessageArguments(arguments, testError), testError);
                REQUIRE(U_SUCCESS(testError));

                std::u16string resultStr(result.getBuffer(), result.length());
                INFO(std::string("Got: ") + inflection::util::StringUtils::to_string(resultStr));
                INFO(std::string("Exp: ") + inflection::util::StringUtils::to_string(print));
                if (hasSpeak) {
                    std::u16string gotPrint;
                    std::u16string gotSpeak;
                    deriveFromSSML(resultStr, &gotPrint, &gotSpeak);
                    INFO(std::string("Got print: ") + inflection::util::StringUtils::to_string(gotPrint));
                    INFO(std::string("Got speak: ") + inflection::util::StringUtils::to_string(gotSpeak));
                    CHECK(gotPrint == print);
                    CHECK(gotSpeak == speakExpected);
                } else {
                    CHECK(resultStr == print);
                }
            }
        }

        xmlFreeDoc(xmlDoc);
        executedFiles++;
    }
    xmlCleanupParser();
    REQUIRE(executedFiles > 0);
}
