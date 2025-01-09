/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <iostream>
#include <map>

TEST_CASE("LanguageGrammarFeaturesTest#testEnglish") /* throws(Exception) */
{
    auto features = inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::LocaleUtils::ENGLISH());
    REQUIRE((features.getCategories()).size() >= 4);
    REQUIRE((features.getFeatures()).size() >= 3);
//    REQUIRE(featuresMap == inflection::lang::features::LanguageGrammarFeatures::createLanguageGrammarFeaturesMap());
}

TEST_CASE("LanguageGrammarFeaturesTest#testConsistency") /* throws(Exception) */
{
    for (const auto& language : ::inflection::lang::features::LanguageGrammarFeatures::getKnownLanguages()) {
        auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(language);
        auto grammarCategories = features.getCategories();
        for (const auto& grammarFeatures : features.getFeatures()) {
            for (const auto& feature : grammarFeatures.getValues()) {
                for (const auto& constraint : feature.getConstraints()) {
                    auto constraintFindIt = grammarCategories.find(constraint.first);
                    REQUIRE(constraintFindIt != grammarCategories.end());
                    auto constraintCategoryValues = (constraintFindIt->second).getValues();
                    REQUIRE(constraintCategoryValues.find(constraint.second) != constraintCategoryValues.end());
                }
            }
        }
    }
}

TEST_CASE("LanguageGrammarFeaturesTest#testRestrictions") /* throws(Exception) */
{
    auto categories(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("es")).getCategories());
    bool foundGenderDependency = false;
    bool foundPersonDependency = false;
    for (const auto& [categoryName, category] : categories) {
        if (categoryName == u"person") {
            auto dependencies(category.getGrammemeDependenciesForCategory());
            auto categoryDependencies = dependencies.equal_range(u"pos");
            REQUIRE(categoryDependencies.first != categoryDependencies.second);
            for (auto it = categoryDependencies.first; it != categoryDependencies.second; ++it) {
                if (it->first == u"pos" && it->second == u"pronoun") {
                    foundPersonDependency = true;
                    break;
                }
            }
        }
        else if (categoryName == u"gender") {
            REQUIRE(category.getGrammemeDependenciesForCategory().empty());
            for (const auto& value : category.getValues()) {
                if (value == u"neuter") {
                    auto valueDependencies(category.getGrammemeDependenciesForValue(value));
                    auto posValueDependencies(valueDependencies.equal_range(u"pos"));
                    REQUIRE(posValueDependencies.first != posValueDependencies.second);
                    for (auto it = posValueDependencies.first; it != posValueDependencies.second; ++it) {
                        if (it->first == u"pos" && it->second == u"pronoun") {
                            foundGenderDependency = true;
                            break;
                        }
                    }
                }
                else {
                    REQUIRE(category.getGrammemeDependenciesForValue(value).empty());
                }
            }
        }
    }
    REQUIRE(foundPersonDependency);
    REQUIRE(foundGenderDependency);
}

static int32_t categorySize(const std::map<std::u16string, inflection::lang::features::LanguageGrammarFeatures_GrammarCategory>& categories, std::u16string_view categoryName) {
    for (const auto& cat : categories) {
        if (cat.second.getName() == categoryName) {
            return (int32_t)cat.second.getValues().size();
        }
    }
    return -1;
}

TEST_CASE("LanguageGrammarFeaturesTest#testLanguageGrammarFeatures") /* throws(Exception) */
{
    REQUIRE(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("en")).getCategories().size() == 12);
    REQUIRE(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("en")).getFeatures().size() == 3);
    auto localeMap(inflection::util::LocaleUtils::getSupportedLocaleMap());
    auto languageFeaturesLanguages(::inflection::lang::features::LanguageGrammarFeatures::getKnownLanguages());
    for (const auto& [language, locales] : localeMap) {
        CHECK(languageFeaturesLanguages.find(::inflection::util::ULocale(language)) != languageFeaturesLanguages.end());
        CHECK_NOTHROW(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale(language)));
    }
    for (const auto& language : languageFeaturesLanguages) {
        CHECK(languageFeaturesLanguages.find(::inflection::util::ULocale(language)) != languageFeaturesLanguages.end());
        CHECK_NOTHROW(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale(language)));
    }
    CHECK_NOTHROW(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::LocaleUtils::ROOT()));
    CHECK_THROWS(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("xx")));
    CHECK_THROWS(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("yy")));
    CHECK_THROWS(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("zz")));
    CHECK_THROWS(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("nn")));

    const auto& nbLangGramFeatures = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::ULocale("nb"));
    auto nbCategories = nbLangGramFeatures.getCategories();
    REQUIRE(nbCategories.size() == 11);
    REQUIRE(categorySize(nbCategories, u"pos") == 10);
    REQUIRE(categorySize(nbCategories, u"number") == 2);

    auto nbFeatures = nbLangGramFeatures.getFeatures();
    REQUIRE(nbFeatures.size() == 9);
    bool foundDefArticle = false;
    bool foundWithDefArticle = false;
    for (const auto& feat : nbFeatures) {
        if (feat.getName() == u"defArticle") {
            REQUIRE(feat.getValues().size() == 4);
            foundDefArticle = true;
        }
        if (feat.getName() == u"withDefArticle") {
            REQUIRE(feat.getValues().empty());
            foundWithDefArticle = true;
        }
    }
    REQUIRE(foundDefArticle);
    REQUIRE_FALSE(foundWithDefArticle);

    const auto& rootLangGramFeatures = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(::inflection::util::LocaleUtils::ROOT());
    auto rootCategories = rootLangGramFeatures.getCategories();
    REQUIRE(rootCategories.size() == 2);
    REQUIRE(categorySize(rootCategories, u"pos") == 10);
    REQUIRE(categorySize(rootCategories, u"gender") == -1);
}

TEST_CASE("LanguageGrammarFeaturesTest#listGrammemes", "[.]")
{
    std::set<std::u16string_view> ignoredCategories({u"possessiveAdj", u"sound"});
    std::set<::inflection::util::ULocale> ignoredLanguages({ // Incompletely vetted languages or not relevant.
        inflection::util::LocaleUtils::INDONESIAN(),
        });
    auto reverseSet(::inflection::lang::features::LanguageGrammarFeatures::getKnownLanguages());
    std::set<::inflection::util::ULocale, std::greater<::inflection::util::ULocale>> languages(reverseSet.begin(), reverseSet.end());
    std::set<::std::u16string> allGrammemes;
    for (const auto& language : languages) {
        if (ignoredLanguages.find(language) != ignoredLanguages.end()) {
            continue;
        }
        std::cout << "language=" << language.getName() << std::endl;
        const auto& langGramFeatures = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(language);
        for (const auto& cat : langGramFeatures.getCategories()) {
            if (ignoredCategories.find(cat.second.getName()) != ignoredCategories.end()) {
                continue;
            }
            std::cout << "    " << inflection::util::StringUtils::to_string(cat.second.getName()) << std::endl;
            for (const auto& value : cat.second.getValues()) {
                std::cout << "        " << inflection::util::StringUtils::to_string(value) << std::endl;
            }
        }
        if (!language.getName().empty()) {
            auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(language);
            std::cout << "    DictionaryMetaData properties" << std::endl;
            for (const auto& propertyName : dictionary->getPropertyNames(-1)) {
                std::cout << "        " << inflection::util::StringUtils::to_string(propertyName) << std::endl;
                allGrammemes.insert(propertyName);
            }
        }
    }
    std::cout << "All known grammemes:" << std::endl;
    for (const auto& grammeme : allGrammemes) {
        std::cout << "    " << inflection::util::StringUtils::to_string(grammeme) << std::endl;
    }
}
