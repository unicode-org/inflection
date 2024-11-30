/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/exception/XMLParseException.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/featureToggle.h>
#include <morphuntion/npc.hpp>
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
        constraintsStr += morphuntion::util::StringUtils::to_string(constraintName) + "=" + morphuntion::util::StringUtils::to_string(constraintValue);
    }
    return constraintsStr;
}

static std::string getInfoStringForTestInput(const ::morphuntion::dialog::SemanticFeatureModel& model, const morphuntion::dialog::SpeakableString& source, const std::string &constraintsStr) {
    return "locale=" + model.getLocale().getName() + (source.isEmpty() ? "" : std::string(" source=") + morphuntion::util::StringUtils::to_string(source.toString())) + " {" + constraintsStr + "}";
}

static void compareInflection(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::morphuntion::dialog::SpeakableString& expected, const ::morphuntion::dialog::SpeakableString& source, const std::map<std::u16string, std::u16string>& constraints, const std::map<std::u16string, std::u16string>& resultAttributes)
{
    morphuntion::dialog::InflectableStringConcept inflectableConcept(&model, source);
    for (const auto& constraint : constraints) {
        auto featureName = model.getFeature(constraint.first);
        if (featureName == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + morphuntion::util::StringUtils::to_string(constraint.first));
        }
        const auto& boundedValues(npc(featureName)->getBoundedValues());
        if (!boundedValues.empty() && boundedValues.find(constraint.second) == boundedValues.end()) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value \"") + morphuntion::util::StringUtils::to_string(constraint.second) + "\" is not valid for feature \"" + morphuntion::util::StringUtils::to_string(constraint.first) + "\"");
        }
        inflectableConcept.putConstraint(*npc(featureName), constraint.second);
    }

    std::optional<bool> isExistsCheck;
    for (const auto& [attrKey, attrValue] : resultAttributes) {
        if (attrKey == u"exists") {
            if ((attrValue != u"true") && (attrValue != u"false")) {
                FAIL_CHECK(model.getLocale().getName() + std::string(": invalid exists value ") + morphuntion::util::StringUtils::to_string(attrValue));
            }
            isExistsCheck = (attrValue == u"true");
            continue;
        }
        const auto &featureName = attrKey;
        const auto &featureValue = attrValue;
        auto semanticFeature = model.getFeature(featureName);
        if (semanticFeature == nullptr) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature name is not recognized: ") + morphuntion::util::StringUtils::to_string(featureName));
        }
        auto result = inflectableConcept.getFeatureValue(*npc(semanticFeature));
        if (result == nullptr && !featureValue.empty()) {
            FAIL_CHECK(model.getLocale().getName() + std::string(": feature value is null for ") + morphuntion::util::StringUtils::to_string(source.toString()) + ::std::string(" when value was expected: ") + morphuntion::util::StringUtils::to_string(featureValue));
        }
        else if (result != nullptr) {
            if (result->speakEqualsPrint() && featureValue == result->getPrint()) {
                SUCCEED("Pass");
            }
            else {
                INFO(model.getLocale().getName() + ", " + morphuntion::util::StringUtils::to_string(semanticFeature->getName()) + ", " + morphuntion::util::StringUtils::to_string(source.toString()));
                CHECK(morphuntion::util::StringUtils::to_string(morphuntion::dialog::SpeakableString(featureValue).toString()) == morphuntion::util::StringUtils::to_string(npc(result)->toString()));
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
            SUCCEED(model.getLocale().getName() + std::string(": correctly got: ") + morphuntion::util::StringUtils::to_string(expected.toString()));
        }
        else {
            auto expectedStr(morphuntion::util::StringUtils::to_string(expected.toString()));
            auto resultStr(morphuntion::util::StringUtils::to_string(npc(result)->toString()));
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

static morphuntion::dialog::SpeakableString getSpeakableString(xmlNodePtr node) {
    xmlNodePtr textNode = xmlFirstElementChild(node);
    if (textNode == nullptr) {
        std::u16string printStr;
        XMLUtils::getContent(&printStr, node);
        return morphuntion::dialog::SpeakableString(printStr);
    }
    if (xmlStrEqual(textNode->name, (const xmlChar *) "text")) {
        std::u16string printStr(XMLUtils::getRequiredAttribute(textNode, "print"));
        std::u16string speakStr(XMLUtils::getRequiredAttribute(textNode, "speak"));
        return {printStr, speakStr};
    }
    throw ::morphuntion::exception::XMLParseException(u"Expecting elements <text>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(textNode->name))) + u">");
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
        if (testLocales.find(locale.getLanguage()) == testLocales.end()) {
            continue;
        }
        auto model = npc(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(locale))->getSemanticFeatureModel();

        numFiles++;
        int32_t numTests = 0;
        for (xmlNodePtr currTestNode = tokenizerTest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <test>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currTestNode->name))) + u">");
            }
            xmlNodePtr curTestChild = xmlFirstElementChild(currTestNode);
            xmlNodePtr sourceNode = curTestChild;
            if (xmlStrEqual(sourceNode->name, (const xmlChar *) "source") == 0) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <source>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(sourceNode->name))) + u">");
            }
            xmlNodePtr resultNode = xmlNextElementSibling(sourceNode);
            if (xmlStrEqual(resultNode->name, (const xmlChar *) "result") == 0) {
                throw ::morphuntion::exception::XMLParseException(u"Expecting element <result>, got <" + ::morphuntion::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(resultNode->name))) + u">");
            }
            auto sourceConstraints(XMLUtils::getAttributes(sourceNode));
            auto resultAttributes(XMLUtils::getAttributes(resultNode));
            auto sourceString(getSpeakableString(sourceNode));
            auto resultString(getSpeakableString(resultNode));

            compareInflection(*npc(model), resultString, sourceString, sourceConstraints, resultAttributes);
            numTests++;
        }
        xmlFreeDoc(xmlDoc);
        REQUIRE(numTests > 0);
    }
    REQUIRE(numFiles > 0);
    xmlCleanupParser();
}
