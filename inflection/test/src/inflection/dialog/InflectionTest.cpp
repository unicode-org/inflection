/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/featureToggle.h>
#include <inflection/npc.hpp>
#include "util/TestUtils.hpp"
#include "util/XMLUtils.hpp"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <algorithm>
#include <set>

static std::string getConstraintString(const std::map<std::u16string, std::u16string>& constraints) {
    std::string constraintsStr;
    for (const auto& [constraintName, constraintValue] : constraints) {
        if (!constraintsStr.empty()) {
            constraintsStr += ',';
        }
        constraintsStr += inflection::util::StringUtils::to_string(constraintName) + "=" + inflection::util::StringUtils::to_string(constraintValue);
    }
    return constraintsStr;
}

static std::string getInfoStringForTestInput(const ::inflection::dialog::SemanticFeatureModel& model, const inflection::dialog::SpeakableString& source, const std::string &constraintsStr) {
    return "locale=" + model.getLocale().getName() + (source.isEmpty() ? "" : std::string(" source=") + inflection::util::StringUtils::to_string(source.toString())) + " {" + constraintsStr + "}";
}

static void compareInflection(const ::inflection::dialog::SemanticFeatureModel& model,
                              const ::inflection::dialog::SpeakableString& expected,
                              const ::inflection::dialog::SpeakableString& source,
                              const std::map<std::u16string, std::u16string>& constraints,
                              const std::map<std::u16string, std::u16string>& resultAttributes,
                              const std::map<std::u16string, std::u16string>& initialAttributes)
{
    std::map<inflection::dialog::SemanticFeature, ::std::u16string> intitialConstraints;
    for(const auto& [attrKey, attrVal] : initialAttributes) {
        auto featureName = model.getFeature(attrKey);
        if (featureName == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + inflection::util::StringUtils::to_string(attrKey));
        }
        const auto& boundedValues(npc(featureName)->getBoundedValues());
        if (!boundedValues.empty() && boundedValues.find(attrVal) == boundedValues.end()) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value \"") + inflection::util::StringUtils::to_string(attrVal) + "\" is not valid for feature \"" + inflection::util::StringUtils::to_string(attrKey) + "\"");
        }
        intitialConstraints[*featureName] = attrVal;
    }
    inflection::dialog::InflectableStringConcept inflectableConcept(&model, source, intitialConstraints);
    for (const auto& constraint : constraints) {
        auto featureName = model.getFeature(constraint.first);
        if (featureName == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + inflection::util::StringUtils::to_string(constraint.first));
        }
        const auto& boundedValues(npc(featureName)->getBoundedValues());
        if (!boundedValues.empty() && boundedValues.find(constraint.second) == boundedValues.end()) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value \"") + inflection::util::StringUtils::to_string(constraint.second) + "\" is not valid for feature \"" + inflection::util::StringUtils::to_string(constraint.first) + "\"");
        }
        inflectableConcept.putConstraint(*npc(featureName), constraint.second);
    }

    std::optional<bool> isExistsCheck;
    for (const auto& [attrKey, attrValue] : resultAttributes) {
        if (attrKey == u"exists") {
            if ((attrValue != u"true") && (attrValue != u"false")) {
                FAIL_CHECK(model.getLocale().getName() + std::string(": invalid exists value ") + inflection::util::StringUtils::to_string(attrValue));
            }
            isExistsCheck = (attrValue == u"true");
            continue;
        }
        const auto &featureName = attrKey;
        const auto &featureValue = attrValue;
        auto semanticFeature = model.getFeature(featureName);
        if (semanticFeature == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + inflection::util::StringUtils::to_string(featureName));
        }
        auto result = inflectableConcept.getFeatureValue(*npc(semanticFeature));
        if (result == nullptr && !featureValue.empty()) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value is null for ") + inflection::util::StringUtils::to_string(source.toString()) + ::std::string(" when value was expected: ") + inflection::util::StringUtils::to_string(featureValue));
        }
        else if (result != nullptr) {
            if (result->speakEqualsPrint() && featureValue == result->getPrint()) {
                SUCCEED("Pass");
            }
            else {
                INFO(model.getLocale().getName() + ", " + inflection::util::StringUtils::to_string(semanticFeature->getName()) + ", " + inflection::util::StringUtils::to_string(source.toString()));
                CHECK(inflection::util::StringUtils::to_string(inflection::dialog::SpeakableString(featureValue).toString()) == inflection::util::StringUtils::to_string(npc(result)->toString()));
            }
        }
        else {
            SUCCEED(model.getLocale().getName() + std::string(": both are null"));
        }
        delete result;
    }

    // Testing isExists
    if (isExistsCheck.has_value()) {
        const auto expectedExists = isExistsCheck.value();
        bool isExists = inflectableConcept.isExists();
        std::string constraintsStr;
        if (expectedExists != isExists) {
            constraintsStr = getConstraintString(constraints);
        }
        INFO(getInfoStringForTestInput(model, source, constraintsStr));
        CHECK(expectedExists == isExists);
    }

    if (!expected.isEmpty()) {
        auto result = inflectableConcept.toSpeakableString();
        if (expected == *npc(result)) {
            SUCCEED(model.getLocale().getName() + std::string(": correctly got: ") + inflection::util::StringUtils::to_string(expected.toString()));
        }
        else {
            auto expectedStr(inflection::util::StringUtils::to_string(expected.toString()));
            auto resultStr(inflection::util::StringUtils::to_string(npc(result)->toString()));
            std::string constraintsStr;
            if (expectedStr != resultStr) {
                constraintsStr = getConstraintString(constraints);
            }
            INFO(getInfoStringForTestInput(model, source, constraintsStr));
            CHECK(expectedStr == resultStr);
        }
        delete result;
    }
}

static inflection::dialog::SpeakableString getSpeakableString(xmlNodePtr node) {
    xmlNodePtr textNode = xmlFirstElementChild(node);
    if (textNode == nullptr) {
        std::u16string printStr;
        XMLUtils::getContent(&printStr, node);
        return inflection::dialog::SpeakableString(printStr);
    }
    if (xmlStrEqual(textNode->name, (const xmlChar *) "text")) {
        std::u16string printStr(XMLUtils::getRequiredAttribute(textNode, "print"));
        std::u16string speakStr(XMLUtils::getRequiredAttribute(textNode, "speak"));
        return {printStr, speakStr};
    }
    throw ::inflection::exception::XMLParseException(u"Expecting elements <text>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(textNode->name))) + u">");
}

TEST_CASE("InflectionTest#testInflections")
{
    const auto &testLocales = TestUtils::getTestLocaleMap();
    std::string resourcePath(TestUtils::getTestResourcePath() + "dialog/inflection/");
    auto files(TestUtils::listDirectoryContents(resourcePath));
    std::sort(files.begin(), files.end());
    int32_t numFiles = 0;
    for (const auto& file : files) {
        ::std::u16string resourceFile;
        ::inflection::util::StringUtils::convert(&resourceFile, resourcePath+file);
        ::inflection::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceFile);
        }
        xmlNodePtr tokenizerTest = xmlDocGetRootElement(xmlDoc);
        if (tokenizerTest == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <inflectionTest>, got <null>");
        }

        ::std::string localeStr = file.substr(0, file.length() - ::std::string(".xml").length());
        ::inflection::util::ULocale locale(localeStr);
        if (testLocales.find(locale.getLanguage()) == testLocales.end()) {
            continue;
        }
        auto model = npc(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale))->getSemanticFeatureModel();

        numFiles++;
        int32_t numTests = 0;
        for (xmlNodePtr currTestNode = tokenizerTest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <test>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currTestNode->name))) + u">");
            }
            xmlNodePtr curTestChild = xmlFirstElementChild(currTestNode);
            xmlNodePtr sourceNode = curTestChild;
            if (xmlStrEqual(sourceNode->name, (const xmlChar *) "source") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <source>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(sourceNode->name))) + u">");
            }

            // optional initial child tag
            curTestChild = xmlNextElementSibling(sourceNode);
            xmlNodePtr initialNode = nullptr;
            if(xmlStrEqual(curTestChild->name, (const xmlChar*) "initial") != 0) {
                initialNode = curTestChild;
                curTestChild = xmlNextElementSibling(curTestChild);
            }

            if (xmlStrEqual(curTestChild->name, (const xmlChar *) "result") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <result>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(curTestChild->name))) + u">");
            }
            xmlNodePtr resultNode = curTestChild;
            auto sourceConstraints(XMLUtils::getAttributes(sourceNode));
            auto resultAttributes(XMLUtils::getAttributes(resultNode));
            auto initialAttributes(XMLUtils::getAttributes(initialNode));
            auto sourceString(getSpeakableString(sourceNode));
            auto resultString(getSpeakableString(resultNode));

            compareInflection(*npc(model), resultString, sourceString, sourceConstraints, resultAttributes, initialAttributes);
            numTests++;
        }
        xmlFreeDoc(xmlDoc);
        REQUIRE(numTests > 0);
    }
    REQUIRE(numFiles > 0);
    xmlCleanupParser();
}
