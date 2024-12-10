/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConcept.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/exception/XMLParseException.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/npc.hpp>
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
    if (!t) throw ::morphuntion::exception::ClassCastException();
    return t;
}

static ::morphuntion::dialog::SpeakableString
formatSource(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr nodePtr);

static inline void
validateNodeName(xmlNodePtr node, const char *name)
{
    if (xmlStrEqual(node->name, (const xmlChar *) name) == 0) {
        throw ::morphuntion::exception::XMLParseException(u"Expecting element <" + ::morphuntion::util::StringUtils::to_u16string(name) + u">, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(node->name))) + u">");
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

/**
 * Returns true if the attribute exists.
 */
static inline bool
getOptionalAttribute(::std::u16string* attributeStr, xmlNodePtr node, const char *attributeName)
{
    xmlChar * attributeValue = xmlGetProp(node, (const xmlChar *)attributeName);
    if (attributeValue != nullptr) {
        ::morphuntion::util::StringUtils::convert(attributeStr, (const char *)attributeValue);
        xmlFree(attributeValue);
        return true;
    }
    return false;
}

template<typename T>
static inline T*
getVariable(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const std::u16string& variableName)
{
    auto variableItr = variables.find(variableName);
    if (variableItr == variables.end()) {
        throw ::morphuntion::exception::XMLParseException(u"Variable is not declared: " + variableName);
    }
    return java_cast<T*>(variableItr->second.get());
}

static void
getContent(::std::u16string* result, xmlNodePtr nodePtr)
{
    xmlChar* content = xmlNodeGetContent(nodePtr);
    ::morphuntion::util::StringUtils::convert(result, (const char*)content);
    xmlFree(content);
}

static ::morphuntion::dialog::NumberConcept*
addNumber(const ::morphuntion::util::ULocale& locale, xmlNodePtr nodePtr)
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
        throw ::morphuntion::exception::XMLParseException(u"Value is missing");
    }
    ::icu4cxx::NumberFormat numParser(UNUM_DECIMAL, ::std::u16string(), ::morphuntion::util::LocaleUtils::ENGLISH().getName().c_str(), nullptr);
    double valDbl = numParser.parseDouble(value, nullptr);
    int64_t val64 = numParser.parseInt64(value, nullptr);
    if (val64 != valDbl) {
        return new ::morphuntion::dialog::NumberConcept(valDbl, locale, locale);
    }
    else {
        return new ::morphuntion::dialog::NumberConcept(val64, locale, locale);
    }
}

static ::morphuntion::dialog::InflectableStringConcept*
addString(const morphuntion::dialog::SemanticFeatureModel& model, xmlNodePtr nodePtr)
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
            throw ::morphuntion::exception::XMLParseException(u"Did not get a constraint for <value>, got <" + constraint + u">");
        }
    }
    if (speak.empty()) {
        return new ::morphuntion::dialog::InflectableStringConcept(&model, morphuntion::dialog::SpeakableString(print));
    }
    else {
        return new ::morphuntion::dialog::InflectableStringConcept(&model, morphuntion::dialog::SpeakableString(print, speak));
    }
}

std::map<morphuntion::dialog::SemanticFeature, std::u16string>
getConstraints(const morphuntion::dialog::SemanticFeatureModel &model, const ::std::set<::std::u16string>& knownCategoryValues, xmlNode *node, const char* propertyName)
{
    std::map<morphuntion::dialog::SemanticFeature, std::u16string> result;
    xmlChar * varInflect = xmlGetProp(node, (const xmlChar *)propertyName);
    if (varInflect != nullptr) {
        ::std::u16string inflectParams;
        morphuntion::util::StringUtils::convert(&inflectParams, (const char *) varInflect);
        for (morphuntion::util::DelimitedStringIterator iterator(inflectParams, u" "); iterator.hasNext(); ++iterator) {
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
                if (semanticFeaturePair == ::std::pair<morphuntion::dialog::SemanticFeature *, ::std::u16string>()) {
                    throw morphuntion::exception::XMLParseException(u"Constraint is not known: " + inflectParams);
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

static ::morphuntion::dialog::SemanticFeatureModel_DisplayData
getDisplayData(const morphuntion::dialog::SemanticFeatureModel &model, const ::std::set<::std::u16string>& knownCategoryValues, xmlNodePtr nodePtr)
{
    std::vector<morphuntion::dialog::SemanticFeatureModel_DisplayValue> displayValues;
    ::std::u16string value;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode, "value");
        auto constraints = getConstraints(model, knownCategoryValues, valueNode, "constraint");
        getContent(&value, valueNode);
        displayValues.emplace_back(morphuntion::dialog::SemanticFeatureModel_DisplayValue(value, constraints));
    }
    return ::morphuntion::dialog::SemanticFeatureModel_DisplayData(displayValues);
}

static ::morphuntion::dialog::SemanticConceptList*
addStringList(const morphuntion::dialog::SemanticFeatureModel& model, xmlNodePtr nodePtr)
{
    ::std::u16string valueStr;
    ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*> values;
    for (xmlNodePtr valueNode = nodePtr->children; valueNode != nullptr; valueNode = valueNode->next) {
        if (valueNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        validateNodeName(valueNode, "value");
        getContent(&valueStr, valueNode);
        values.emplace_back(new ::morphuntion::dialog::InflectableStringConcept(&model, morphuntion::dialog::SpeakableString(valueStr)));
    }
    auto result = new ::morphuntion::dialog::SemanticConceptList(&model, values);
    for (auto value : values) {
        delete value;
    }
    return result;
}

static ::morphuntion::dialog::SpeakableString
formatVariable(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, xmlNodePtr sourceNode)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    xmlChar * varNameXML = xmlGetProp(sourceNode, (const xmlChar *)"name");
    ::std::u16string varName;
    ::morphuntion::util::StringUtils::convert(&varName, (const char *)varNameXML);
    auto variable = getVariable<morphuntion::dialog::SpeakableConcept>(variables, varName);

    auto constraints = getConstraints(model, knownCategoryValues, sourceNode, "inflect");
    if (!constraints.empty()) {
        morphuntion::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<morphuntion::dialog::SemanticFeatureConceptBase*>(variable);
        std::unique_ptr<::morphuntion::dialog::SpeakableString> varResult;
        const morphuntion::dialog::SemanticFeature* semanticFeaturePtr = nullptr;
        for (const auto& constraint : constraints) {
            if (constraint.second.empty()) {
                if (semanticFeaturePtr != nullptr) {
                    throw ::morphuntion::exception::XMLParseException(u"Can not request more than one non-grammatical category");
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
        std::unique_ptr<morphuntion::dialog::SpeakableString> speakableString(npc(npc(variable)->toSpeakableString()));
        result = *speakableString;
    }
    xmlFree(varNameXML);
    return result;
}

static ::morphuntion::dialog::SpeakableString
formatNumber(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, xmlNodePtr sourceNode)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    auto variable = getVariable<morphuntion::dialog::NumberConcept>(variables, getRequiredAttribute(sourceNode, "name"));

    xmlChar * styleStr = xmlGetProp(sourceNode, (const xmlChar *)"style");
    xmlChar * variantStr = xmlGetProp(sourceNode, (const xmlChar *)"variant");
    std::u16string argument;
    if (variantStr != nullptr) {
        morphuntion::util::StringUtils::convert(&argument, (const char*)variantStr);
    }
    if (styleStr != nullptr) {
        if (xmlStrEqual(styleStr, (const xmlChar *) "asWords")) {
            if (variantStr == nullptr) {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->getAsWords());
            }
            else {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->asWords(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asSpokenWords")) {
            if (variantStr == nullptr) {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->getAsSpokenWords());
            }
            else {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->asSpokenWords(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asDigits")) {
            if (variantStr == nullptr) {
                result = npc(variable)->getAsDigits();
            }
            else {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->asDigits(argument));
            }
        }
        else if (xmlStrEqual(styleStr, (const xmlChar *) "asOrdinalDigits")) {
            if (variantStr == nullptr) {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->getAsOrdinalDigits());
            }
            else {
                result = ::morphuntion::dialog::SpeakableString(npc(variable)->asDigits(argument));
            }
        }
        else {
            throw ::morphuntion::exception::XMLParseException(u"Style is not recognized: " + morphuntion::util::StringUtils::to_u16string((const char*)styleStr));
        }
    }
    else {
        std::unique_ptr<morphuntion::dialog::SpeakableString> speakableString(npc(npc(variable)->toSpeakableString()));
        result = *speakableString;
    }
    xmlFree(styleStr);
    xmlFree(variantStr);
    return result;
}

static ::morphuntion::dialog::SpeakableString
formatList(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr sourceNode)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    auto variableOriginal = getVariable<morphuntion::dialog::SemanticConceptList>(variables, getRequiredAttribute(sourceNode, "name"));
    std::unique_ptr<morphuntion::dialog::SemanticConceptList> variable;
    ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*> items;
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
        throw ::morphuntion::exception::XMLParseException(u"Unrecognized list type: " + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(typeNameXML))));
    }
    xmlFree(typeNameXML);
    typeNameXML = nullptr;

    auto constraints = getConstraints(model, knownCategoryValues, sourceNode, "inflect");
    if (!constraints.empty()) {
        morphuntion::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<morphuntion::dialog::SemanticFeatureConceptBase*>(variable.get());
        std::unique_ptr<::morphuntion::dialog::SpeakableString> varResult;
        const morphuntion::dialog::SemanticFeature* semanticFeaturePtr = nullptr;
        for (const auto& constraint : constraints) {
            if (constraint.second.empty()) {
                if (semanticFeaturePtr != nullptr) {
                    throw ::morphuntion::exception::XMLParseException(u"Can not request more than one non-grammatical category");
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
        std::unique_ptr<morphuntion::dialog::SpeakableString> speakableString(npc(variable->toSpeakableString()));
        result = *speakableString;
    }
    for (auto item : items) {
        delete item;
    }
    return result;
}

static ::morphuntion::dialog::SpeakableString
formatSwitch(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr switchNode)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    ::morphuntion::dialog::SpeakableString valueToCheck(u"");
    auto variable = getVariable<morphuntion::dialog::SpeakableConcept>(variables, getRequiredAttribute(switchNode, "value"));

    std::unique_ptr<::morphuntion::dialog::SpeakableString> valueForComparison;
    xmlChar* featureAttribute = xmlGetProp(switchNode, (const xmlChar *)"feature");
    if (featureAttribute != nullptr) {
        ::std::u16string feature;
        ::morphuntion::util::StringUtils::convert(&feature, (const char *)featureAttribute);
        xmlFree(featureAttribute);
        featureAttribute = nullptr;
        auto semanticFeaturePtr = model.getFeature(feature);
        if (semanticFeaturePtr == nullptr) {
            throw ::morphuntion::exception::XMLParseException(u"Semantic feature is not known: " + feature);
        }

        morphuntion::dialog::SemanticFeatureConceptBase* conceptVar = java_cast<morphuntion::dialog::SemanticFeatureConceptBase*>(variable);
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
                    throw ::morphuntion::exception::XMLParseException(u"Value to check should not be null");
                }
                ::std::u16string valueToCheckPropertyStr;
                morphuntion::util::StringUtils::convert(&valueToCheckPropertyStr, (const char*)valueToCheckProperty);
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
            throw ::morphuntion::exception::XMLParseException(u"Expecting element <case> or <default>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currSwitchNode->name))) + u">");
        }
    }

    return result;
}

static ::morphuntion::dialog::SpeakableString
formatQuantity(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr quantityNode)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    ::morphuntion::dialog::SpeakableString valueToCheck(u"");
    auto valueObj = getVariable<morphuntion::dialog::NumberConcept>(variables, getRequiredAttribute(quantityNode, "value"));

    std::u16string varName = getRequiredAttribute(quantityNode, "unit");
    auto unitObj = getVariable<morphuntion::dialog::SemanticFeatureConceptBase>(variables, varName);
    auto constraints = getConstraints(model, knownCategoryValues, quantityNode, "inflect");
    if (!constraints.empty()) {
        std::unique_ptr<::morphuntion::dialog::SpeakableString> varResult;
        for (const auto &constraint : constraints) {
            if (constraint.second.empty()) {
                throw ::morphuntion::exception::XMLParseException(u"Can not request semantic feature: " + constraint.first.getName());
            } else {
                npc(unitObj)->putConstraint(constraint.first, constraint.second);
            }
        }
    }

    std::unique_ptr<morphuntion::dialog::SpeakableString> formattedResult(npc(npc(commonConceptFactory)->quantify(*npc(valueObj), unitObj)));
    result = *formattedResult;

    return result;
}

static ::morphuntion::dialog::SpeakableString
formatSource(const ::std::map<::std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>>& variables, const ::std::set<::std::u16string>& knownCategoryValues, const morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::CommonConceptFactory* commonConceptFactory, xmlNodePtr nodePtr)
{
    ::morphuntion::dialog::SpeakableString result(u"");
    ::std::u16string text;
    for (xmlNodePtr sourceNode = nodePtr->children; sourceNode != nullptr; sourceNode = sourceNode->next) {
        if (sourceNode->type == XML_TEXT_NODE) {
            getContent(&text, sourceNode);
            result += ::morphuntion::dialog::SpeakableString(text);
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
                throw ::morphuntion::exception::XMLParseException(u"Type is not recognized " + morphuntion::util::StringUtils::to_u16string((const char *)sourceNode->name));
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
    for (const auto& locale : ::morphuntion::util::LocaleUtils::getSupportedLocaleList()) {
        const morphuntion::dialog::CommonConceptFactory* commonConceptFactory = nullptr;
        try {
            commonConceptFactory = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale));
        }
        catch (const morphuntion::exception::RuntimeException& e) {
            throw;
        }

        std::string resourcePath = TestUtils::getTestResourcePath() + std::string("dialog/format/") + locale.getName() + ".xml";
        struct stat buffer;
        if (stat(resourcePath.c_str(), &buffer) != 0) {
            // Ignore missing files
            continue;
        }
        ::std::u16string resourceFile;
        morphuntion::util::StringUtils::convert(&resourceFile, resourcePath);
        ::morphuntion::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::morphuntion::exception::IOException(resourceFile);
        }
        xmlNodePtr formattest = xmlDocGetRootElement(xmlDoc);
        if (formattest == nullptr) {
            throw ::morphuntion::exception::XMLParseException(u"Incorrect root element -- expecting <formattest>, got <null>");
        }
        validateNodeName(formattest, "formattest");
        auto features = ::morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
        ::std::set<::std::u16string> knownCategoryValues;
        for (const auto& grammarCategory : features.getCategories()) {
            for (const auto& value : grammarCategory.second.getValues()) {
                knownCategoryValues.insert(value);
            }
        }

        morphuntion::dialog::SemanticFeatureModel model(locale);
        ::std::map<std::u16string, std::unique_ptr<morphuntion::dialog::SpeakableConcept>> variables;
        ::std::map<morphuntion::dialog::SemanticValue, morphuntion::dialog::SemanticFeatureModel_DisplayData> semanticValueMap;

        for (xmlNodePtr currTestNode = formattest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <pattern>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currTestNode->name))) + u">");
            }

            ::morphuntion::dialog::SpeakableString source(u"");
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
                            throw ::morphuntion::exception::XMLParseException(u"Expecting param. Attribute is not recognized " + morphuntion::util::StringUtils::to_u16string((const char *)testParamNode->name));
                        }
                        xmlChar * paramName = xmlGetProp(testParamNode, (const xmlChar *)"name");
                        ::std::u16string paramNameStr;
                        ::morphuntion::util::StringUtils::convert(&paramNameStr, (const char*)paramName);
                        xmlChar * paramType = xmlGetProp(testParamNode, (const xmlChar *)"type");
                        if (xmlStrEqual(paramType, (const xmlChar *) "String")) {
                            variables.emplace(paramNameStr, std::unique_ptr<morphuntion::dialog::SpeakableConcept>((morphuntion::dialog::SemanticFeatureConceptBase*)addString(model, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "String[]")) {
                            variables.emplace(paramNameStr, std::unique_ptr<morphuntion::dialog::SpeakableConcept>((morphuntion::dialog::SemanticFeatureConceptBase*)addStringList(model, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "Number")) {
                            variables.emplace(paramNameStr, std::unique_ptr<morphuntion::dialog::SpeakableConcept>((morphuntion::dialog::SemanticFeatureConceptBase*)addNumber(locale, testParamNode)));
                        }
                        else if (xmlStrEqual(paramType, (const xmlChar *) "SemanticConcept")) {
                            semanticValueMap.emplace(morphuntion::dialog::SemanticValue(u"", paramNameStr), getDisplayData(model, knownCategoryValues, testParamNode));
                        }
                        else {
                            throw ::morphuntion::exception::XMLParseException(u"Type is not recognized " + morphuntion::util::StringUtils::to_u16string((const char *)paramType));
                        }
                        xmlFree(paramName);
                        xmlFree(paramType);
                    }
                    continue;
                }
                else if (xmlStrEqual(testPropertyNode->name, (const xmlChar *) "source")) {
                    morphuntion::dialog::SemanticFeatureModel modelWithSemanticValues(locale, semanticValueMap);
                    for (const auto& entry : semanticValueMap) {
                        variables.emplace(entry.first.getValue(), std::unique_ptr<morphuntion::dialog::SpeakableConcept>((morphuntion::dialog::SemanticFeatureConceptBase*)new ::morphuntion::dialog::SemanticConcept(&modelWithSemanticValues, entry.first)));
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
            INFO(std::string("Got: ") + morphuntion::util::StringUtils::to_string(source.getPrint()));
            INFO(std::string("Exp: ") + morphuntion::util::StringUtils::to_string(print));
            CHECK(source.getPrint() == print);
            if (!speak.empty()) {
                INFO(std::string("Got: ") + morphuntion::util::StringUtils::to_string(source.getSpeak()));
                INFO(std::string("Exp: ") + morphuntion::util::StringUtils::to_string(speak));
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
