/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConcept.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/npc.hpp>
#include "util/TestUtils.hpp"
#include <memory>

static ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> createConstraints(const ::morphuntion::dialog::SemanticFeatureModel* model, const ::std::vector<::std::u16string>& namedValues)
{
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> constraintMap;
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::morphuntion::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
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
    ::morphuntion::dialog::SemanticFeatureModel originalModel(::morphuntion::util::LocaleUtils::ENGLISH());
    ::std::map<::morphuntion::dialog::SemanticValue, ::morphuntion::dialog::SemanticFeatureModel_DisplayData> semanticValueMap({
        {::morphuntion::dialog::SemanticValue(u"default", u"val1"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({})},
        {::morphuntion::dialog::SemanticValue(u"default", u"val2"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {u"number=plural"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"val3"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {u"number=singular"})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {u"number=plural"}))
        })},
        {::morphuntion::dialog::SemanticValue(u"default", u"val4"), ::morphuntion::dialog::SemanticFeatureModel_DisplayData({
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.1", createConstraints(&originalModel, {})),
            ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(u"val2.2", createConstraints(&originalModel, {}))
        })},
    });
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second == semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second);
    REQUIRE_FALSE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second != semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second);
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second != semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second);
    REQUIRE_FALSE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val1"))->second == semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second);
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second == semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val3"))->second);
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second != semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val4"))->second);
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] == semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[0]);
    REQUIRE_FALSE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] != semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[0]);
    REQUIRE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] != semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[1]);
    REQUIRE_FALSE(semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val2"))->second.getValues()[0] == semanticValueMap.find(::morphuntion::dialog::SemanticValue(u"default", u"val3"))->second.getValues()[1]);
}

TEST_CASE("SemanticFeatureModelTest#testSemanticFeature")
{
    ::std::unique_ptr<::morphuntion::dialog::SemanticFeature> semanticFeature(morphuntion::dialog::SemanticFeature::noValue(u"noValue"));
    REQUIRE(semanticFeature->getType() == morphuntion::dialog::SemanticFeature::Type::NoValue);
    REQUIRE(semanticFeature->getBoundedValues().empty());

    semanticFeature.reset(morphuntion::dialog::SemanticFeature::boundedValue(u"boundedValue", {u"1", u"2"}));
    REQUIRE(semanticFeature->getType() == morphuntion::dialog::SemanticFeature::Type::BoundedValue);
    REQUIRE(semanticFeature->getBoundedValues() == std::set<std::u16string>({u"1", u"2"}));

    semanticFeature.reset(morphuntion::dialog::SemanticFeature::unboundedValue(u"unboundedValue"));
    REQUIRE(semanticFeature->getType() == morphuntion::dialog::SemanticFeature::Type::UnboundedValue);
    REQUIRE(semanticFeature->getBoundedValues().empty());
}

TEST_CASE("SemanticFeatureModelTest#testAliasing")
{
    for (const auto& [languageStr, locale] : morphuntion::util::LocaleUtils::getSupportedLocaleMap()) {
        morphuntion::util::ULocale language(languageStr);
        try {
            ::morphuntion::dialog::SemanticFeatureModel model(language);
            const auto& langGramFeatures = ::morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(language);
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
                        CHECK(morphuntion::util::StringUtils::to_string(npc(aliasFeature)->getName()) == morphuntion::util::StringUtils::to_string(categoryName));
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
//                        WARN(morphuntion::util::StringUtils::to_string(featureAsCategoryValue->getName()));
//                    }
//                    CHECK(featureAsCategoryValue == nullptr);
//                }
            }
        }
        catch (const morphuntion::exception::RuntimeException& e) {
            throw;
        }
    }
}

TEST_CASE("SemanticFeatureModelTest#testAllSupportedSucceedConstruction")
{
    for (const auto& locale : morphuntion::util::LocaleUtils::getSupportedLocaleList()) {
        INFO(locale.getName());
        CHECK_NOTHROW(::morphuntion::dialog::SemanticFeatureModel(locale));
    }
    // We do not support Klingon.
    CHECK_THROWS(::morphuntion::dialog::SemanticFeatureModel(morphuntion::util::ULocale("tlh")));
}
