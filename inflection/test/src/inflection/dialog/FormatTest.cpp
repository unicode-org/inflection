/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConcept.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/NumberFormat.hpp>
#include "util/TestUtils.hpp"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <sys/stat.h>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

static ::inflection::dialog::SpeakableString
formatSource(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr nodePtr);

static inline void
validateNodeName(xmlNodePtr node, const char *name)
{
    if (xmlStrEqual(node->name, (const xmlChar *) name) == 0) {
        throw ::inflection::exception::XMLParseException(u"Expecting element <" + ::inflection::util::StringUtils::to_u16string(name) + u">, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(node->name))) + u">");
    }
}

static inline std::u16string
getRequiredAttribute(xmlNodePtr node, const char *attributeName)
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

/**
 * Returns true if the attribute exists.
 */
static inline bool
getOptionalAttribute(::std::u16string* attributeStr, xmlNodePtr node, const char *attributeName)
{
    xmlChar * attributeValue = xmlGetProp(node, (const xmlChar *)attributeName);
    if (attributeValue != nullptr) {
        ::inflection::util::StringUtils::convert(attributeStr, (const char *)attributeValue);
        xmlFree(attributeValue);
        return true;
    }
    return false;
}

template<typename T>
static inline T*
getVariable(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const std::u16string& variableName)
{
    auto variableItr = variables.find(variableName);
    if (variableItr == variables.end()) {
        throw ::inflection::exception::XMLParseException(u"Variable is not declared: " + variableName);
    }
    return java_cast<T*>(variableItr->second.get());
}

static void
getContent(::std::u16string* result, xmlNodePtr nodePtr)
{
    xmlChar* content = xmlNodeGetContent(nodePtr);
    ::inflection::util::StringUtils::convert(result, (const char*)content);
    xmlFree(content);
}

static ::inflection::dialog::NumberConcept*
addNumber(const ::inflection::util::ULocale& locale, xmlNodePtr nodePtr)
{
    ::std::u16string value;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode, "value");
        getContent(&value, valueNode);
    }
    if (value.empty()) {
        throw ::inflection::exception::XMLParseException(u"Value is missing");
    }
    ::icu4cxx::NumberFormat numParser(UNUM_DECIMAL, ::std::u16string(), ::inflection::util::LocaleUtils::ENGLISH().getName().c_str(), nullptr);
    double valDbl = numParser.parseDouble(value, nullptr);
    int64_t val64 = numParser.parseInt64(value, nullptr);
    if (val64 != valDbl) {
        return new ::inflection::dialog::NumberConcept(valDbl, locale, locale);
    }
    else {
        return new ::inflection::dialog::NumberConcept(val64, locale, locale);
    }
}

static ::inflection::dialog::InflectableStringConcept*
addString(const inflection::dialog::SemanticFeatureModel& model, xmlNodePtr nodePtr)
{
    ::std::u16string print;
    ::std::u16string speak;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode,"value");
        std::u16string constraint;
        getOptionalAttribute(&constraint, valueNode, "constraint");
        if (constraint.empty()) {
            getContent(&print, valueNode);
        }
        else if (constraint == u"speak") {
            getContent(&speak, valueNode);
        }
        else {
            throw ::inflection::exception::XMLParseException(u"Did not get a constraint for <value>, got <" + constraint + u">");
        }
    }
    if (speak.empty()) {
        return new ::inflection::dialog::InflectableStringConcept(&model, inflection::dialog::SpeakableString(print));
    }
    else {
        return new ::inflection::dialog::InflectableStringConcept(&model, inflection::dialog::SpeakableString(print, speak));
    }
}

std::map<inflection::dialog::SemanticFeature, std::u16string>
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
            if (knownCategoryValues.find(constraint) == knownCategoryValues.end()) {
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
        displayValues.emplace_back(inflection::dialog::DisplayValue(value, constraints));
    }
    return ::inflection::dialog::SemanticFeatureModel_DisplayData(displayValues);
}

static ::inflection::dialog::SemanticConceptList*
addStringList(const inflection::dialog::SemanticFeatureModel& model, xmlNodePtr nodePtr)
{
    ::std::u16string valueStr;
    ::std::vector<::inflection::dialog::SemanticFeatureConceptBase*> values;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode, "value");
        getContent(&valueStr, valueNode);
        values.emplace_back(new ::inflection::dialog::InflectableStringConcept(&model, inflection::dialog::SpeakableString(valueStr)));
    }
    auto result = new ::inflection::dialog::SemanticConceptList(&model, values);
    for (auto value : values) {
        delete value;
    }
    return result;
}

static ::inflection::dialog::SpeakableString
formatVariable(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, xmlNodePtr sourceNode)
{
    ::inflection::dialog::SpeakableString result(u"");
    xmlChar * varNameXML = xmlGetProp(sourceNode, (const xmlChar *)"name");
    ::std::u16string varName;
    ::inflection::util::StringUtils::convert(&varName, (const char *)varNameXML);
    auto variable = getVariable<inflection::dialog::SpeakableConcept>(variables, varName);

    auto constraints = getConstraints(model, knownCategoryValues, sourceNode, "inflect");
    if (!constraints.empty()) {
        inflection::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<inflection::dialog::SemanticFeatureConceptBase*>(variable);
        std::unique_ptr<::inflection::dialog::SpeakableString> varResult;
        const inflection::dialog::SemanticFeature* semanticFeaturePtr = nullptr;
        for (const auto& constraint : constraints) {
            if (constraint.second.empty()) {
                if (semanticFeaturePtr != nullptr) {
                    throw ::inflection::exception::XMLParseException(u"Can not request more than one non-grammatical category");
                }
                semanticFeaturePtr = &constraint.first;
            }
            else {
                npc(conceptVar)->putConstraint(constraint.first, constraint.second);
            }
        }
        if (semanticFeaturePtr != nullptr) {
            varResult.reset(npc(npc(conceptVar)->getFeatureValue(*npc(semanticFeaturePtr))));
        }
        else {
            varResult.reset(npc(npc(conceptVar)->toSpeakableString()));
            npc(conceptVar)->reset();
        }
        result = *varResult;
    }
    else {
        std::unique_ptr<inflection::dialog::SpeakableString> speakableString(npc(npc(variable)->toSpeakableString()));
        result = *speakableString;
    }
    xmlFree(varNameXML);
    return result;
}

static ::inflection::dialog::SpeakableString
formatNumber(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, xmlNodePtr sourceNode)
{
    ::inflection::dialog::SpeakableString result(u"");
    auto variable = getVariable<inflection::dialog::NumberConcept>(variables, getRequiredAttribute(sourceNode, "name"));

    xmlChar * styleStr = xmlGetProp(sourceNode, (const xmlChar *)"style");
    xmlChar * variantStr = xmlGetProp(sourceNode, (const xmlChar *)"variant");
    std::u16string argument;
    if (variantStr != nullptr) {
        inflection::util::StringUtils::convert(&argument, (const char*)variantStr);
    }
    if (styleStr != nullptr) {
        if (xmlStrEqual(styleStr, (const xmlChar *) "asWords")) {
            if (variantStr == nullptr) {
                result = ::inflection::dialog::SpeakableString(npc(variable)->getAsWords());
            }
            else {
                result = ::inflection::dialog::SpeakableString(npc(variable)->asWords(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asSpokenWords")) {
            if (variantStr == nullptr) {
                result = ::inflection::dialog::SpeakableString(npc(variable)->getAsSpokenWords());
            }
            else {
                result = ::inflection::dialog::SpeakableString(npc(variable)->asSpokenWords(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asDigits")) {
            if (variantStr == nullptr) {
                result = npc(variable)->getAsDigits();
            }
            else {
                result = ::inflection::dialog::SpeakableString(npc(variable)->asDigits(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asOrdinalDigits")) {
            if (variantStr == nullptr) {
                result = ::inflection::dialog::SpeakableString(npc(variable)->getAsOrdinalDigits());
            }
            else {
                result = ::inflection::dialog::SpeakableString(npc(variable)->asDigits(argument));
            }
        }
        else {
            throw ::inflection::exception::XMLParseException(u"Style is not recognized: " + inflection::util::StringUtils::to_u16string((const char*)styleStr));
        }
    }
    else {
        std::unique_ptr<inflection::dialog::SpeakableString> speakableString(npc(npc(variable)->toSpeakableString()));
        result = *speakableString;
    }
    xmlFree(styleStr);
    xmlFree(variantStr);
    return result;
}

static ::inflection::dialog::SpeakableString
formatList(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr sourceNode)
{
    ::inflection::dialog::SpeakableString result(u"");
    auto variableOriginal = getVariable<inflection::dialog::SemanticConceptList>(variables, getRequiredAttribute(sourceNode, "name"));
    std::unique_ptr<inflection::dialog::SemanticConceptList> variable;
    ::std::vector<::inflection::dialog::SemanticFeatureConceptBase*> items;
    int32_t size = npc(variableOriginal)->getSize();
    for (int32_t idx = 0; idx < size; idx++) {
        items.emplace_back(npc(npc(variableOriginal)->get(idx))->clone());
    }

    xmlChar * typeNameXML = xmlGetProp(sourceNode, (const xmlChar *)"type");
    if (xmlStrEqual(typeNameXML, (const xmlChar *) "conjunction")) {
        variable.reset(npc(npc(commonConceptFactory)->createAndList(items)));
    }
    else if (xmlStrEqual(typeNameXML, (const xmlChar *) "disjunction")) {
        variable.reset(npc(npc(commonConceptFactory)->createOrList(items)));
    }
    else {
        throw ::inflection::exception::XMLParseException(u"Unrecognized list type: " + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(typeNameXML))));
    }
    xmlFree(typeNameXML);
    typeNameXML = nullptr;

    auto constraints = getConstraints(model, knownCategoryValues, sourceNode, "inflect");
    if (!constraints.empty()) {
        inflection::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<inflection::dialog::SemanticFeatureConceptBase*>(variable.get());
        std::unique_ptr<::inflection::dialog::SpeakableString> varResult;
        const inflection::dialog::SemanticFeature* semanticFeaturePtr = nullptr;
        for (const auto& constraint : constraints) {
            if (constraint.second.empty()) {
                if (semanticFeaturePtr != nullptr) {
                    throw ::inflection::exception::XMLParseException(u"Can not request more than one non-grammatical category");
                }
                semanticFeaturePtr = &constraint.first;
            }
            else {
                npc(conceptVar)->putConstraint(constraint.first, constraint.second);
            }
        }
        if (semanticFeaturePtr != nullptr) {
            varResult.reset(npc(npc(conceptVar)->getFeatureValue(*npc(semanticFeaturePtr))));
        }
        else {
            varResult.reset(npc(npc(conceptVar)->toSpeakableString()));
            npc(conceptVar)->reset();
        }
        result = *varResult;
    }
    else {
        std::unique_ptr<inflection::dialog::SpeakableString> speakableString(npc(variable->toSpeakableString()));
        result = *speakableString;
    }
    for (auto item : items) {
        delete item;
    }
    return result;
}

static ::inflection::dialog::SpeakableString
formatSwitch(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr switchNode)
{
    ::inflection::dialog::SpeakableString result(u"");
    ::inflection::dialog::SpeakableString valueToCheck(u"");
    auto variable = getVariable<inflection::dialog::SpeakableConcept>(variables, getRequiredAttribute(switchNode, "value"));

    std::unique_ptr<::inflection::dialog::SpeakableString> valueForComparison;
    xmlChar* featureAttribute = xmlGetProp(switchNode, (const xmlChar *)"feature");
    if (featureAttribute != nullptr) {
        ::std::u16string feature;
        ::inflection::util::StringUtils::convert(&feature, (const char *)featureAttribute);
        xmlFree(featureAttribute);
        featureAttribute = nullptr;
        auto semanticFeaturePtr = model.getFeature(feature);
        if (semanticFeaturePtr == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Semantic feature is not known: " + feature);
        }

        inflection::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<inflection::dialog::SemanticFeatureConceptBase*>(variable);
        valueForComparison.reset(npc(npc(conceptVar)->getFeatureValue(*npc(semanticFeaturePtr))));
    }
    else {
        valueForComparison.reset(npc(npc(variable)->toSpeakableString()));
    }

    for (xmlNodePtr currSwitchNode = switchNode->children; currSwitchNode != nullptr; currSwitchNode = currSwitchNode->next) {
        if (currSwitchNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(currSwitchNode->name, (const xmlChar *) "case")) {
            if (valueForComparison != nullptr) {
                xmlChar * valueToCheckProperty = xmlGetProp(currSwitchNode, (const xmlChar *)"is");
                if (valueToCheckProperty == nullptr) {
                    throw ::inflection::exception::XMLParseException(u"Value to check should not be null");
                }
                ::std::u16string valueToCheckPropertyStr;
                inflection::util::StringUtils::convert(&valueToCheckPropertyStr, (const char*)valueToCheckProperty);
                xmlFree(valueToCheckProperty);
                if (valueForComparison->getPrint() == valueToCheckPropertyStr) {
                    result = formatSource(variables, knownCategoryValues, model, commonConceptFactory, currSwitchNode);
                    break;
                }
            }
        }
        else if (xmlStrEqual(currSwitchNode->name, (const xmlChar *) "default")) {
            result = formatSource(variables, knownCategoryValues, model, commonConceptFactory, currSwitchNode);
            break;
        }
        else {
            throw ::inflection::exception::XMLParseException(u"Expecting element <case> or <default>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currSwitchNode->name))) + u">");
        }
    }

    return result;
}

static ::inflection::dialog::SpeakableString
formatQuantity(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr quantityNode)
{
    ::inflection::dialog::SpeakableString result(u"");
    ::inflection::dialog::SpeakableString valueToCheck(u"");
    auto valueObj = getVariable<inflection::dialog::NumberConcept>(variables, getRequiredAttribute(quantityNode, "value"));

    std::u16string varName = getRequiredAttribute(quantityNode, "unit");
    auto unitObj = getVariable<inflection::dialog::SemanticFeatureConceptBase>(variables, varName);
    auto constraints = getConstraints(model, knownCategoryValues, quantityNode, "inflect");
    if (!constraints.empty()) {
        std::unique_ptr<::inflection::dialog::SpeakableString> varResult;
        for (const auto &constraint : constraints) {
            if (constraint.second.empty()) {
                throw ::inflection::exception::XMLParseException(u"Can not request semantic feature: " + constraint.first.getName());
            } else {
                npc(unitObj)->putConstraint(constraint.first, constraint.second);
            }
        }
    }

    std::unique_ptr<inflection::dialog::SpeakableString> formattedResult(npc(npc(commonConceptFactory)->quantify(*npc(valueObj), unitObj)));
    result = *formattedResult;

    return result;
}

static ::inflection::dialog::SpeakableString
formatSource(const ::std::map<::std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr nodePtr)
{
    ::inflection::dialog::SpeakableString result(u"");
    ::std::u16string text;
    for (xmlNodePtr sourceNode = nodePtr->children; sourceNode != nullptr; sourceNode = sourceNode->next) {
        if (sourceNode->type == XML_TEXT_NODE) {
            getContent(&text, sourceNode);
            result += ::inflection::dialog::SpeakableString(text);
        }
        else if (sourceNode->type == XML_ELEMENT_NODE) {
            if (xmlStrEqual(sourceNode->name, (const xmlChar *) "var")) {
                result += formatVariable(variables, knownCategoryValues, model, sourceNode);
            }
            else if (xmlStrEqual(sourceNode->name, (const xmlChar *) "switch")) {
                result += formatSwitch(variables, knownCategoryValues, model, commonConceptFactory, sourceNode);
            }
            else if (xmlStrEqual(sourceNode->name, (const xmlChar *) "quantity")) {
                result += formatQuantity(variables, knownCategoryValues, model, commonConceptFactory, sourceNode);
            }
            else if (xmlStrEqual(sourceNode->name, (const xmlChar *) "number")) {
                result += formatNumber(variables, sourceNode);
            }
            else if (xmlStrEqual(sourceNode->name, (const xmlChar *) "list")) {
                result += formatList(variables, knownCategoryValues, model, commonConceptFactory, sourceNode);
            }
            else {
                throw ::inflection::exception::XMLParseException(u"Type is not recognized " + inflection::util::StringUtils::to_u16string((const char *)sourceNode->name));
            }
        }
    }
    return result;
}

TEST_CASE("FormatTest#testLocales")
{
    int32_t executedFiles = 0;
    ::std::u16string print;
    ::std::u16string speak;
    for (const auto& locale : ::inflection::util::LocaleUtils::getSupportedLocaleList()) {
        const inflection::dialog::CommonConceptFactory* commonConceptFactory = nullptr;
        try {
            commonConceptFactory = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale));
        }
        catch (const inflection::exception::RuntimeException& e) {
            throw;
        }

        std::string resourcePath = TestUtils::getTestResourcePath() + std::string("dialog/format/") + locale.getName() + ".xml";
        struct stat buffer;
        if (stat(resourcePath.c_str(), &buffer) != 0) {
            // Ignore missing files
            continue;
        }
        ::std::u16string resourceFile;
        inflection::util::StringUtils::convert(&resourceFile, resourcePath);
        ::inflection::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceFile);
        }
        xmlNodePtr formattest = xmlDocGetRootElement(xmlDoc);
        if (formattest == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <formattest>, got <null>");
        }
        validateNodeName(formattest, "formattest");
        auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
        ::std::set<::std::u16string> knownCategoryValues;
        for (const auto& grammarCategory : features.getCategories()) {
            for (const auto& value : grammarCategory.second.getValues()) {
                knownCategoryValues.insert(value);
            }
        }

        inflection::dialog::SemanticFeatureModel model(locale);
        ::std::map<std::u16string, std::unique_ptr<inflection::dialog::SpeakableConcept>> variables;
        ::std::map<inflection::dialog::SemanticValue, inflection::dialog::SemanticFeatureModel_DisplayData> semanticValueMap;

        for (xmlNodePtr currTestNode = formattest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <pattern>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currTestNode->name))) + u">");
            }

            ::inflection::dialog::SpeakableString source(u"");
            print.clear();
            speak.clear();
            variables.clear();

            for (xmlNodePtr testPropertyNode = currTestNode->children; testPropertyNode != nullptr; testPropertyNode = testPropertyNode->next) {
                if (testPropertyNode->type != XML_ELEMENT_NODE) {
                    continue;
                }
                if (xmlStrEqual(testPropertyNode->name, (const xmlChar *) "params")) {
                    for (xmlNodePtr testParamNode = testPropertyNode->children; testParamNode != nullptr; testParamNode = testParamNode->next) {
                        if (testParamNode->type != XML_ELEMENT_NODE) {
                            continue;
                        }
                        if (xmlStrEqual(testParamNode->name, (const xmlChar *) "param") == 0) {
                            throw ::inflection::exception::XMLParseException(u"Expecting param. Attribute is not recognized " + inflection::util::StringUtils::to_u16string((const char *)testParamNode->name));
                        }
                        xmlChar * paramName = xmlGetProp(testParamNode, (const xmlChar *)"name");
                        ::std::u16string paramNameStr;
                        ::inflection::util::StringUtils::convert(&paramNameStr, (const char*)paramName);
                        xmlChar * paramType = xmlGetProp(testParamNode, (const xmlChar *)"type");
                        if (xmlStrEqual(paramType, (const xmlChar *) "String")) {
                            variables.emplace(paramNameStr, std::unique_ptr<inflection::dialog::SpeakableConcept>((inflection::dialog::SemanticFeatureConceptBase*)addString(model, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "String[]")) {
                            variables.emplace(paramNameStr, std::unique_ptr<inflection::dialog::SpeakableConcept>((inflection::dialog::SemanticFeatureConceptBase*)addStringList(model, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "Number")) {
                            variables.emplace(paramNameStr, std::unique_ptr<inflection::dialog::SpeakableConcept>((inflection::dialog::SemanticFeatureConceptBase*)addNumber(locale, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "SemanticConcept")) {
                            semanticValueMap.emplace(inflection::dialog::SemanticValue(u"", paramNameStr), getDisplayData(model, knownCategoryValues, testParamNode));
                        }
                        else {
                            throw ::inflection::exception::XMLParseException(u"Type is not recognized " + inflection::util::StringUtils::to_u16string((const char *)paramType));
                        }
                        xmlFree(paramName);
                        xmlFree(paramType);
                    }
                    continue;
                }
                else if (xmlStrEqual(testPropertyNode->name, (const xmlChar *) "source")) {
                    inflection::dialog::SemanticFeatureModel modelWithSemanticValues(locale, semanticValueMap);
                    for (const auto& entry : semanticValueMap) {
                        variables.emplace(entry.first.getValue(), std::unique_ptr<inflection::dialog::SpeakableConcept>((inflection::dialog::SemanticFeatureConceptBase*)new ::inflection::dialog::SemanticConcept(&modelWithSemanticValues, entry.first)));
                    }
                    source = formatSource(variables, knownCategoryValues, modelWithSemanticValues, commonConceptFactory, testPropertyNode);
                    continue;
                }
                else if (xmlStrEqual(testPropertyNode->name, (const xmlChar *) "print")) {
                    getContent(&print, testPropertyNode);
                    continue;
                }
                else if (xmlStrEqual(testPropertyNode->name, (const xmlChar *) "speak")) {
                    getContent(&speak, testPropertyNode);
                    continue;
                }
                else {
                    FAIL(::std::string("Unknown property ") + ::std::string((const char *)testPropertyNode->name));
                }
            }
            INFO(std::string("Got: ") + inflection::util::StringUtils::to_string(source.getPrint()));
            INFO(std::string("Exp: ") + inflection::util::StringUtils::to_string(print));
            CHECK(source.getPrint() == print);
            if (!speak.empty()) {
                INFO(std::string("Got: ") + inflection::util::StringUtils::to_string(source.getSpeak()));
                INFO(std::string("Exp: ") + inflection::util::StringUtils::to_string(speak));
                CHECK(source.getSpeak() == speak);
            }
            variables.clear();
        }

        xmlFreeDoc(xmlDoc);
        executedFiles++;
    }
    xmlCleanupParser();
    REQUIRE(executedFiles > 0);
}
