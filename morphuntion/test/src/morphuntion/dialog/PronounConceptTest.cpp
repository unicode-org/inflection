/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/PronounConcept.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/exception/XMLParseException.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>
#include <util/TestUtils.hpp>
#include <dirent.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <algorithm>
#include <memory>
#include <set>

static const morphuntion::dialog::SemanticFeature* getFeature(const ::morphuntion::dialog::SemanticFeatureModel& model, const std::u16string& semanticName, const std::u16string& semanticValue) {
    auto featureName = model.getFeature(semanticName);
    if (featureName == nullptr) {
        FAIL_CHECK(std::string("Feature name is not recognized: ") + morphuntion::util::StringUtils::to_string(semanticName));
    }
    const auto& boundedValues(npc(featureName)->getBoundedValues());
    if (!boundedValues.empty() && boundedValues.find(semanticValue) == boundedValues.end()) {
        FAIL_CHECK(std::string("Feature value is not valid: ") + morphuntion::util::StringUtils::to_string(semanticName) + '=' + morphuntion::util::StringUtils::to_string(semanticValue));
    }
    return featureName;
}

static ::morphuntion::dialog::SemanticFeatureModel_DisplayValue createDisplayValue(const ::morphuntion::dialog::SemanticFeatureModel& model, const std::u16string& displayValue, const std::map<std::u16string, std::u16string>& constraints) {
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> semanticConstraints;
    for (const auto& [key, value] : constraints) {
        semanticConstraints.emplace(*npc(getFeature(model, key, value)), value);
    }
    return {displayValue, semanticConstraints};
}

static constexpr char16_t DEPENDENCY_SOUND[] = u"dependency=sound";

static void compareInflection(morphuntion::dialog::PronounConcept& pronounConcept, const ::morphuntion::dialog::SemanticFeatureModel& model, const ::morphuntion::dialog::SpeakableString& expected, const ::morphuntion::dialog::SpeakableString& source, const std::map<std::u16string, std::u16string>& constraints, const std::map<std::u16string, std::u16string>& featureValues)
{
    std::u16string possesseeString;
    bool hasPossesseeConcept = false;
    const auto& soundConstraint = constraints.find(DEPENDENCY_SOUND);
    if (soundConstraint != constraints.end() && (soundConstraint->second == u"vowel-start" || soundConstraint->second == u"vowel-end")) {
        possesseeString = u"a";
        hasPossesseeConcept = true;
    }
    morphuntion::dialog::InflectableStringConcept possesseeConcept(&model, morphuntion::dialog::SpeakableString(possesseeString));
    for (const auto& [key, value] : constraints) {
        auto equalsIdx = key.find('=');
        if (equalsIdx == std::u16string::npos) {
            pronounConcept.putConstraint(*npc(getFeature(model, key, value)), value);
        }
        else {
            hasPossesseeConcept = true;
            if (key != DEPENDENCY_SOUND) {
                possesseeConcept.putConstraint(*npc(getFeature(model, key.substr(equalsIdx + 1), value)), value);
            }
            // else it's already handled.
        }
    }
    for (const auto& featureValue : featureValues) {
        auto semanticFeature = model.getFeature(featureValue.first);
        if (semanticFeature == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + morphuntion::util::StringUtils::to_string(featureValue.first));
        }
        auto result = pronounConcept.getFeatureValue(*npc(semanticFeature));
        if (result == nullptr && !featureValue.second.empty()) {
            auto sourcePrint(source.getPrint());
            if (!sourcePrint.empty()) {
                sourcePrint = u" Source=\"" + sourcePrint + u"\" ";
            }
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value is null when value was expected: ") + morphuntion::util::StringUtils::to_string(featureValue.second) + morphuntion::util::StringUtils::to_string(sourcePrint));
        }
        else if (result != nullptr) {
            if (result->speakEqualsPrint() && featureValue.second == result->getPrint()) {
                SUCCEED("Pass");
            }
            else {
                INFO(model.getLocale().getName() + ", " + morphuntion::util::StringUtils::to_string(semanticFeature->getName()) + ", " + morphuntion::util::StringUtils::to_string(source.toString()));
                FAIL_CHECK(morphuntion::util::StringUtils::to_string(expected.toString() + u" property " + morphuntion::dialog::SpeakableString(featureValue.second).toString() + u"!=" + result->toString()));
            }
        }
        else {
            SUCCEED(model.getLocale().getName() + std::string(": both are null"));
        }
        delete result;
    }
    if (!expected.isEmpty()) {
        auto result = hasPossesseeConcept ? pronounConcept.toSpeakableString(possesseeConcept) : pronounConcept.toSpeakableString();
        if (expected == *npc(result)) {
            SUCCEED(model.getLocale().getName() + std::string(": correctly got: ") + morphuntion::util::StringUtils::to_string(expected.toString()));
        }
        else {
            auto expectedStr(morphuntion::util::StringUtils::to_string(expected.toString()));
            auto resultStr(morphuntion::util::StringUtils::to_string(npc(result)->toString()));
            std::string constraintsStr;
            if (expectedStr != resultStr) {
                for (const auto& [key, value] : constraints) {
                    if (!constraintsStr.empty()) {
                        constraintsStr += ',';
                    }
                    constraintsStr += morphuntion::util::StringUtils::to_string(key) + "=" + morphuntion::util::StringUtils::to_string(value);
                }
            }
            INFO("locale=" + model.getLocale().getName() + (source.isEmpty() ? "" : std::string(" source=") + morphuntion::util::StringUtils::to_string(source.toString())) + " {" + constraintsStr + "}");
            CHECK(expectedStr == resultStr);
        }
        delete result;
    }
}

static void compareInflection(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::morphuntion::dialog::SpeakableString& expected, const ::morphuntion::dialog::SpeakableString& source, const std::map<std::u16string, std::u16string>& constraints, const std::map<std::u16string, std::u16string>& featureValues)
{
    ::std::unique_ptr<morphuntion::dialog::PronounConcept> pronounConcept(npc(source.isEmpty()
                                                                          ? new morphuntion::dialog::PronounConcept(model, ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>())
                                                                          : new morphuntion::dialog::PronounConcept(model, source.getPrint())));
    compareInflection(*pronounConcept.get(), model, expected, source, constraints, featureValues);
}

static std::map<std::u16string, std::u16string> getAttributes(xmlNodePtr node) {
    xmlAttr* attribute = node->properties;
    std::map<std::u16string, std::u16string> result;
    while (attribute) {
        xmlChar* value = xmlNodeListGetString(node->doc, attribute->children, 1);
        result.emplace(morphuntion::util::StringUtils::to_u16string((const char*)(attribute->name)), morphuntion::util::StringUtils::to_u16string((const char*)(value)));
        xmlFree(value);
        attribute = attribute->next;
    }
    return result;
}

static void
getContent(::std::u16string* result, const xmlNodePtr nodePtr)
{
    xmlChar* content = xmlNodeGetContent(nodePtr);
    if (content != nullptr) {
        ::morphuntion::util::StringUtils::convert(result, (const char*)content);
        xmlFree(content);
    }
    else {
        npc(result)->clear();
    }
}

static inline std::u16string
getRequiredAttribute(xmlNodePtr node, const char *attributeName)
{
    xmlChar * attributeValue = xmlGetProp(node, (const xmlChar *)attributeName);
    if (attributeValue == nullptr) {
        throw ::morphuntion::exception::XMLParseException(std::u16string(u"The attribute ") + ::morphuntion::util::StringUtils::to_u16string(attributeName) + u" is required on " + ::morphuntion::util::StringUtils::to_u16string((const char *)npc(node)->name));
    }
    ::std::u16string attributeStr;
    ::morphuntion::util::StringUtils::convert(&attributeStr, (const char *)attributeValue);
    xmlFree(attributeValue);
    return attributeStr;
}

static morphuntion::dialog::SpeakableString getSpeakableString(xmlNodePtr node) {
    xmlNodePtr textNode = xmlFirstElementChild(node);
    if (textNode == nullptr) {
        std::u16string printStr;
        getContent(&printStr, node);
        return morphuntion::dialog::SpeakableString(printStr);
    }
    if (xmlStrEqual(textNode->name, (const xmlChar *) "text")) {
        std::u16string printStr(getRequiredAttribute(textNode, "print"));
        std::u16string speakStr(getRequiredAttribute(textNode, "speak"));
        return morphuntion::dialog::SpeakableString(printStr, speakStr);
    }
    throw ::morphuntion::exception::XMLParseException(u"Expecting elements <text>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string((const char*)(textNode->name))) + u">");
}

static ::std::vector<::std::string> listDirectoryContents(const std::string& dirPath)
{
    ::std::vector<::std::string> files;
    DIR* dirptr = opendir(dirPath.c_str());
    if (dirptr == nullptr) {
        FAIL(std::string("Not a valid path: ") + dirPath);
    }
    const struct dirent * dirEnt;
    while ((dirEnt = readdir(dirptr)) != nullptr) {
        if (strcmp(dirEnt->d_name, ".") == 0 || strcmp(dirEnt->d_name, "..") == 0) {
            continue;
        }
        files.emplace_back(dirEnt->d_name);
    }
    closedir(dirptr);
    return files;
}

TEST_CASE("PronounConceptTest#testInflections")
{
    std::string resourcePath(TestUtils::getTestResourcePath() + "dialog/pronoun/");
    auto files(listDirectoryContents(resourcePath));
    sort(files.begin(), files.end());
    const ::std::u16string dependencyPrefix(u"dependency=");
    int32_t numFiles = 0;
    for (auto const& file : files) {
        ::std::u16string resourceFile;
        ::morphuntion::util::StringUtils::convert(&resourceFile, resourcePath+file);
        ::morphuntion::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::morphuntion::exception::IOException(resourceFile);
        }
        xmlNodePtr tokenizerTest = xmlDocGetRootElement(xmlDoc);
        if (tokenizerTest == nullptr) {
            throw ::morphuntion::exception::XMLParseException(u"Incorrect root element -- expecting <inflectionTest>, got <null>");
        }

        ::std::string localeStr = file.substr(0, file.length() - ::std::string(".xml").length());
        ::morphuntion::util::ULocale locale(localeStr);
        auto model = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale))->getSemanticFeatureModel();

        numFiles++;
        int32_t numTests = 0;
        for (xmlNodePtr currTestNode = tokenizerTest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <test>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string((const char*)(currTestNode->name))) + u">");
            }
            xmlNodePtr sourceNode = xmlFirstElementChild(currTestNode);
            if (!xmlStrEqual(sourceNode->name, (const xmlChar *) "source")) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <source>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string((const char*)(sourceNode->name))) + u">");
            }
            xmlNodePtr codependentNode = xmlNextElementSibling(sourceNode);
            xmlNodePtr resultNode = codependentNode;
            if (!xmlStrEqual(npc(codependentNode)->name, (const xmlChar *) "codependent")) {
                codependentNode = nullptr;
            }
            else {
                // optional
                resultNode = xmlNextElementSibling(codependentNode);
            }
            if (!xmlStrEqual(npc(resultNode)->name, (const xmlChar *) "result")) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <result>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string((const char*)(resultNode->name))) + u">");
            }
            auto sourceConstraints(getAttributes(sourceNode));
            auto codependentConstraints(codependentNode == nullptr ? std::map<std::u16string, std::u16string>() : getAttributes(codependentNode));
            auto resultAttributes(getAttributes(resultNode));
            auto sourceString(getSpeakableString(sourceNode));
            auto resultString(getSpeakableString(resultNode));

            for (const auto& [key, value] : codependentConstraints) {
                sourceConstraints.emplace(dependencyPrefix + key, value);
            }

            compareInflection(*npc(model), resultString, sourceString, sourceConstraints, resultAttributes);
            numTests++;
        }
        xmlFreeDoc(xmlDoc);
        REQUIRE(numTests > 0);
    }
    REQUIRE(numFiles > 0);
    xmlCleanupParser();
}

TEST_CASE("PronounConceptTest#testBadArgument")
{
    auto model = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(morphuntion::util::LocaleUtils::ENGLISH()))->getSemanticFeatureModel();
    REQUIRE_THROWS(morphuntion::dialog::PronounConcept(*npc(model), u"garbage")); // This is an unrecognized pronoun to start from.
    REQUIRE_THROWS(morphuntion::dialog::PronounConcept(morphuntion::dialog::SemanticFeatureModel(morphuntion::util::ULocale("tlh")), ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>())); // We do not recognize Klingon pronouns
    REQUIRE_THROWS(morphuntion::dialog::PronounConcept(morphuntion::dialog::SemanticFeatureModel(morphuntion::util::ULocale("")), ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>())); // We do not recognize root pronouns right now
}

TEST_CASE("PronounConceptTest#testExistence")
{
    const auto& model = *npc(npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(morphuntion::util::LocaleUtils::ENGLISH()))->getSemanticFeatureModel());
    morphuntion::dialog::PronounConcept pronounConcept(model, ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>());
    pronounConcept.putConstraintByName(u"person", u"second");
    pronounConcept.putConstraintByName(u"number", u"singular");
    CHECK(pronounConcept.isExists());
    ::std::unique_ptr<morphuntion::dialog::SpeakableString> result(npc(pronounConcept.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"you")); // This is a little under specified

    pronounConcept.putConstraintByName(u"definiteness", u"definite");
    CHECK_FALSE(pronounConcept.isExists());
    result.reset(npc(pronounConcept.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"they")); // There is no definite form.
}

TEST_CASE("PronounConceptTest#testCustom")
{
    const auto& model = *npc(npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(morphuntion::util::LocaleUtils::ENGLISH()))->getSemanticFeatureModel());
    ::std::vector<::morphuntion::dialog::SemanticFeatureModel_DisplayValue> defaultDisplayData({
        createDisplayValue(model, u"y'all", {{u"person", u"second"}, {u"number", u"plural"}, {u"case", u"nominative"}})
    });
    morphuntion::dialog::PronounConcept pronounConcept(model, defaultDisplayData, {});

    pronounConcept.putConstraintByName(u"person", u"second");
    ::std::unique_ptr<morphuntion::dialog::SpeakableString> result(npc(pronounConcept.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"y'all")); // This is a little under specified. So we're starting with the plural one.

    pronounConcept.putConstraintByName(u"number", u"singular");
    result.reset(npc(pronounConcept.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"you"));

    pronounConcept.putConstraintByName(u"number", u"plural");
    result.reset(npc(pronounConcept.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"y'all"));

    morphuntion::dialog::PronounConcept pronounConcept2(model, defaultDisplayData, {{*getFeature(model, u"number", u"singular"), u"singular"}});

    pronounConcept2.putConstraintByName(u"person", u"second");
    result.reset(npc(pronounConcept2.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"you")); // This is a little under specified, but the defaults favor singular

    pronounConcept2.putConstraintByName(u"number", u"singular");
    result.reset(npc(pronounConcept2.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"you"));

    pronounConcept2.putConstraintByName(u"number", u"plural");
    result.reset(npc(pronounConcept2.toSpeakableString()));
    CHECK(*result == morphuntion::dialog::SpeakableString(u"y'all"));

    CHECK(pronounConcept == pronounConcept);
    CHECK(pronounConcept != pronounConcept2); // The defaults are the different
    ::std::unique_ptr<morphuntion::dialog::PronounConcept> clone(npc(pronounConcept.clone()));
    CHECK(pronounConcept == *clone);
    pronounConcept.putConstraintByName(u"person", u"first");
    CHECK(pronounConcept != *clone); // The constraints are the different
}

TEST_CASE("PronounConceptTest#testFullSupport")
{
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> emptyMap;
    for (const auto& locale : morphuntion::util::LocaleUtils::getSupportedLocaleList()) {
        CHECK_NOTHROW(morphuntion::dialog::PronounConcept(morphuntion::dialog::SemanticFeatureModel(locale), emptyMap));
        CHECK_NOTHROW(delete morphuntion::dialog::PronounConcept(morphuntion::dialog::SemanticFeatureModel(locale), emptyMap).toSpeakableString());
    }
}

TEST_CASE("PronounConceptTest#testFullAccess")
{
    std::string resourcePath;
    ::std::string srcDir(__FILE__);
    srcDir = srcDir.substr(0, srcDir.length() - ::std::string("/test/src/morphuntion/dialog/PronounConceptTest.cpp").length());
    resourcePath = srcDir + "/resources/org/unicode/morphuntion/inflection/";

    auto files(listDirectoryContents(resourcePath));
    sort(files.begin(), files.end());
    int32_t numFiles = 0;
    const ::morphuntion::dialog::SpeakableString empty(u"");
    const ::std::u16string_view dependencyPrefix(u"dependency=");
    ::std::map<std::u16string, std::u16string> constraints;
    ::std::map<std::u16string, std::u16string> codependentConstraints;
    ::std::u16string constraint;

    for (auto const& file : files) {
        if (file.rfind(".properties") != std::string::npos) {
            continue;
        }
        auto underscore = file.find('_');
        auto dot = file.find('.');
        if (underscore == std::string::npos || dot == std::string::npos) {
            FAIL("File can not parse the locale from " + file);
            return;
        }
        underscore++;
        std::string localeStr(file.substr(underscore, dot - underscore));
        morphuntion::util::ULocale locale(localeStr);
        const auto& model = *npc(npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale))->getSemanticFeatureModel());

        ::std::u16string resourceFile;
        ::morphuntion::util::StringUtils::convert(&resourceFile, resourcePath + file);
        ::morphuntion::util::MemoryMappedFile mMapFile(resourceFile);
        ::std::u16string reader(morphuntion::util::StringViewUtils::to_u16string(std::string_view(mMapFile.getData(), mMapFile.getSize())));
        morphuntion::dialog::PronounConcept pronounConcept(model, ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>());
        for (::morphuntion::util::DelimitedStringIterator iterator(reader, u"\n"); iterator.hasNext(); ++iterator) {
            ::std::u16string word;
            constraints.clear();
            for (::morphuntion::util::DelimitedStringIterator cell(*iterator, u","); cell.hasNext(); ++cell) {
                if (word.empty()) {
                    word = *cell;
                }
                else {
                    constraint = *cell;
                    const morphuntion::dialog::SemanticFeature* semanticFeature = nullptr;
                    if (morphuntion::util::StringViewUtils::startsWith(constraint, dependencyPrefix)) {
                        auto constraintValue(constraint.substr(dependencyPrefix.length()));
                        if (constraintValue != u"sound") {
                            auto featureAlias = model.getFeatureAlias(constraintValue);
                            if (featureAlias.first == nullptr) {
                                auto equalsIdx = constraintValue.find(u'=');
                                if (equalsIdx != std::u16string::npos) {
                                    featureAlias.second = constraintValue.substr(equalsIdx + 1);
                                    constraintValue = constraintValue.substr(0, equalsIdx);
                                }
                                semanticFeature = model.getFeature(constraintValue);
                                if (semanticFeature == nullptr) {
                                    FAIL("Unknown dependency constraint for PronounConcept: " + morphuntion::util::StringViewUtils::to_string(constraint));
                                }
                            }
                            else {
                                semanticFeature = featureAlias.first;
                            }
                            if (!featureAlias.second.empty()) {
                                constraints.emplace(std::u16string(dependencyPrefix) + semanticFeature->getName(), featureAlias.second);
                            }
                            // else TODO Don't skip all possible values in the future.
                        }
                    }
                    else {
                        auto featureAlias = model.getFeatureAlias(constraint);
                        if (featureAlias.first == nullptr) {
                            auto equalsIdx = constraint.find(u'=');
                            if (equalsIdx != std::u16string::npos) {
                                featureAlias.second = constraint.substr(equalsIdx + 1);
                                constraint = constraint.substr(0, equalsIdx);
                            }
                            // else featureAlias.second will have an empty string to represent all values.
                            semanticFeature = model.getFeature(constraint);
                            if (semanticFeature == nullptr) {
                                FAIL("Unknown constraint for PronounConcept: " + morphuntion::util::StringViewUtils::to_string(constraint));
                            }
                        }
                        else {
                            semanticFeature = featureAlias.first;
                        }
                        if (!featureAlias.second.empty()) {
                            constraints.emplace(semanticFeature->getName(), featureAlias.second);
                        }
                        // else TODO Don't skip all possible values in the future.
                    }
                }
            }
            pronounConcept.reset();
            compareInflection(pronounConcept, model, morphuntion::dialog::SpeakableString(word), empty, constraints, {});
        }
        numFiles++;
    }
    REQUIRE(numFiles > 0);
}

TEST_CASE("PronounConceptTest#testElision")
{
    auto model = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(morphuntion::util::LocaleUtils::FRENCH()))->getSemanticFeatureModel();
    morphuntion::dialog::PronounConcept pronounConcept(*npc(model), ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>());
    morphuntion::dialog::InflectableStringConcept vowelConcept(model, morphuntion::dialog::SpeakableString(u"appareil"));
    morphuntion::dialog::InflectableStringConcept consonantConcept(model, morphuntion::dialog::SpeakableString(u"lumière"));
    morphuntion::dialog::InflectableStringConcept emptyConcept(model, morphuntion::dialog::SpeakableString(u""));

    pronounConcept.putConstraintByName(u"person", u"second");
    pronounConcept.putConstraintByName(u"number", u"singular");
    pronounConcept.putConstraintByName(u"case", u"accusative");

    ::std::unique_ptr<morphuntion::dialog::SpeakableString> result(npc(pronounConcept.toSpeakableString()));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(vowelConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"t’").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(consonantConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(emptyConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));

    pronounConcept.clearConstraintByName(u"number");
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(vowelConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"t’").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(consonantConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));

    result.reset(npc(pronounConcept.toSpeakableString(emptyConcept)));
    CHECK(morphuntion::util::StringViewUtils::to_string(result->toString()) == morphuntion::util::StringViewUtils::to_string(morphuntion::dialog::SpeakableString(u"te ").toString()));
}
