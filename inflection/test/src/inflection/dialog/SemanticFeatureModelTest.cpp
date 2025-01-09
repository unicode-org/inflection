/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConcept.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>
#include "util/TestUtils.hpp"
#include <memory>

static ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> createConstraints(const ::inflection::dialog::SemanticFeatureModel* model, const ::std::vector<::std::u16string>& namedValues)
{
    ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> constraintMap;
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::inflection::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                value = *iterator;
            }
        }
        constraintMap.emplace(*npc(npc(model)->getFeature(key)), value);
    }
    return constraintMap;
}

TEST_CASE("SemanticFeatureModelTest#testEquality")
{
    ::inflection::dialog::SemanticFeatureModel originalModel(::inflection::util::LocaleUtils::ENGLISH());
    ::std::map<::inflection::dialog::SemanticValue, ::inflection::dialog::SemanticFeatureModel_DisplayData> semanticValueMap({
        {::inflection::dialog::SemanticValue(u"default", u"val1"), ::inflection::dialog::SemanticFeatureModel_DisplayData({})},
        {::inflection::dialog::SemanticValue(u"default", u"val2"), ::inflection::dialog::SemanticFeatureModel_DisplayData({
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {u"number=singular"})),
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {u"number=plural"}))
        })},
        {::inflection::dialog::SemanticValue(u"default", u"val3"), ::inflection::dialog::SemanticFeatureModel_DisplayData({
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {u"number=singular"})),
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {u"number=plural"}))
        })},
        {::inflection::dialog::SemanticValue(u"default", u"val4"), ::inflection::dialog::SemanticFeatureModel_DisplayData({
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {})),
            ::inflection::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {}))
        })},
    });
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second == semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second);
    REQUIRE_FALSE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second != semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second);
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second != semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second);
    REQUIRE_FALSE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val1"))->second == semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second);
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second == semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val3"))->second);
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second != semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val4"))->second);
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] == semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[0]);
    REQUIRE_FALSE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] != semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[0]);
    REQUIRE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] != semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[1]);
    REQUIRE_FALSE(semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] == semanticValueMap.find(::inflection::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[1]);
}

TEST_CASE("SemanticFeatureModelTest#testSemanticFeature")
{
    ::std::unique_ptr<::inflection::dialog::SemanticFeature> semanticFeature(inflection::dialog::SemanticFeature::noValue(u"noValue"));
    REQUIRE(semanticFeature->getType() == inflection::dialog::SemanticFeature::Type::NoValue);
    REQUIRE(semanticFeature->getBoundedValues().empty());

    semanticFeature.reset(inflection::dialog::SemanticFeature::boundedValue(u"boundedValue", {u"1", u"2"}));
    REQUIRE(semanticFeature->getType() == inflection::dialog::SemanticFeature::Type::BoundedValue);
    REQUIRE(semanticFeature->getBoundedValues() == std::set<std::u16string>({u"1", u"2"}));

    semanticFeature.reset(inflection::dialog::SemanticFeature::unboundedValue(u"unboundedValue"));
    REQUIRE(semanticFeature->getType() == inflection::dialog::SemanticFeature::Type::UnboundedValue);
    REQUIRE(semanticFeature->getBoundedValues().empty());
}

TEST_CASE("SemanticFeatureModelTest#testAliasing")
{
    for (const auto& [languageStr, locale] : inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        inflection::util::ULocale language(languageStr);
        try {
            ::inflection::dialog::SemanticFeatureModel model(language);
            const auto& langGramFeatures = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(language);
            for (const auto& [categoryName, category] : langGramFeatures.getCategories()) {
                auto feature = npc(model.getFeature(categoryName));
                INFO(languageStr);
                CHECK(feature->getBoundedValues() == category.getValues());
                if (category.isUniqueValues()) {
                    CHECK(feature->isAliased());
                    for (const auto &value : category.getValues()) {
                        auto [aliasFeature, resolvedName] = model.getFeatureAlias(value);
                        // This better not get mapped to something else.
                        CHECK(resolvedName == value);
                        CHECK(inflection::util::StringUtils::to_string(npc(aliasFeature)->getName()) == inflection::util::StringUtils::to_string(categoryName));
                    }
                }
                else {
                    CHECK_FALSE(feature->isAliased());
                    for (const auto &value : category.getValues()) {
                        auto [aliasFeature, resolvedName] = model.getFeatureAlias(value);
                        // It's either not aliased, or it's found to map to something else.
                        CHECK((resolvedName.empty() || resolvedName == value));
                        CHECK((aliasFeature == nullptr || npc(aliasFeature)->getName() != categoryName));
                    }
                }
                // TODO Wait for the problematic conflicting semantic feature names to stop being used, and remove them from grammar.xml.
//                for (const auto& value : category.getValues()) {
//                    auto featureAsCategoryValue = model.getFeature(value);
//                    if (featureAsCategoryValue != nullptr) {
//                        WARN(inflection::util::StringUtils::to_string(featureAsCategoryValue->getName()));
//                    }
//                    CHECK(featureAsCategoryValue == nullptr);
//                }
            }
        }
        catch (const inflection::exception::RuntimeException& e) {
            throw;
        }
    }
}

TEST_CASE("SemanticFeatureModelTest#testAllSupportedSucceedConstruction")
{
    for (const auto& locale : inflection::util::LocaleUtils::getSupportedLocaleList()) {
        INFO(locale.getName());
        CHECK_NOTHROW(::inflection::dialog::SemanticFeatureModel(locale));
    }
    // We do not support Klingon.
    CHECK_THROWS(::inflection::dialog::SemanticFeatureModel(inflection::util::ULocale("tlh")));
}
