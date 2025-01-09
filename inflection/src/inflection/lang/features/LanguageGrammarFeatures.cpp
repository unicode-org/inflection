/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>

#include <inflection/lang/features/LanguageGrammarFeatures_Data.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_Feature.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarFeatures.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/Object.hpp>
#include <inflection/npc.hpp>

namespace inflection::lang::features {

LanguageGrammarFeatures::LanguageGrammarFeatures(const ::inflection::util::ULocale& pLanguage, const Grammar& grammar, const Features& features)
    : super()
    , language(pLanguage)
    , grammar(grammar)
    , features(features)
{
}

LanguageGrammarFeatures::~LanguageGrammarFeatures()
{
}

LanguageGrammarFeatures LanguageGrammarFeatures::getLanguageGrammarFeatures(const ::inflection::util::ULocale& locale)
{
    if (locale.getName().empty()) {
        return LanguageGrammarFeatures(locale, GRAMMAR_XML.common.grammar, GRAMMAR_XML.common.features);
    }
    std::u16string languageStr;
    inflection::util::StringViewUtils::convert(&languageStr, locale.getLanguage());
    for (int32_t i = 0; i < GRAMMAR_XML.languageCount; ++i) {
        if (languageStr == GRAMMAR_XML.languages[i].id) {
            return LanguageGrammarFeatures(locale, GRAMMAR_XML.languages[i].grammar, GRAMMAR_XML.languages[i].features);
        }
    }

    throw ::inflection::exception::IllegalArgumentException(u"Language features can not be found for language " + locale.toString());
}

std::set<::inflection::util::ULocale> LanguageGrammarFeatures::getKnownLanguages()
{
    std::set<::inflection::util::ULocale> languages;
    for (int32_t i = 0; i < GRAMMAR_XML.languageCount; ++i) {
        languages.emplace(inflection::util::StringViewUtils::to_string(GRAMMAR_XML.languages[i].id));
    }
    languages.emplace(inflection::util::LocaleUtils::ROOT()); // common

    return languages;
}

inflection::util::ULocale LanguageGrammarFeatures::getLanguage() const
{
    return language;
}

int32_t LanguageGrammarFeatures::getCategoriesCount() const
{
    return grammar.categoryCount;
}

void LanguageGrammarFeatures::addCategory(std::map<std::u16string, LanguageGrammarFeatures_GrammarCategory>* categories, const Category& category)
{
    std::set<std::u16string> values;
    std::map<std::u16string_view, std::multimap<std::u16string, std::u16string>> grammemeDependenciesForValues;
    for (int32_t j = 0; j < category.grammemeCount; ++j)
    {
        values.emplace(category.grammemes[j].name);
        for (int32_t k = 0; k < category.grammemes[j].restrictions.restrictionsCount; ++k)
        {
            grammemeDependenciesForValues[category.grammemes[j].name].emplace(category.grammemes[j].restrictions.restrictions[k].name, category.grammemes[j].restrictions.restrictions[k].grammeme);
        }
    }
    std::multimap<std::u16string, std::u16string> grammemeDependenciesForCategory;
    for (int32_t j = 0; j < category.restrictions.restrictionsCount; ++j)
    {
        grammemeDependenciesForCategory.emplace(category.restrictions.restrictions[j].name, category.restrictions.restrictions[j].grammeme);
    }

    auto categoryName = category.name;
    auto [categoryItr, categoryMissing] = npc(categories)->insert({categoryName, {categoryName, values, grammemeDependenciesForCategory, grammemeDependenciesForValues}});
    if (!categoryMissing) { // category already exists
        categoryItr->second.values.insert(values.begin(), values.end());
        categoryItr->second.grammemeDependenciesForCategory.insert(grammemeDependenciesForCategory.begin(), grammemeDependenciesForCategory.end());
        categoryItr->second.grammemeDependenciesForValues.insert(grammemeDependenciesForValues.begin(), grammemeDependenciesForValues.end());
    } else {
        categoryItr->second.uniqueValues = category.aliasable;
    }
}

std::map<std::u16string, LanguageGrammarFeatures_GrammarCategory> LanguageGrammarFeatures::getCategories() const
{
    std::map<std::u16string, LanguageGrammarFeatures_GrammarCategory> categories;

    // add categories from common
    for (int32_t i = 0; i < GRAMMAR_XML.common.grammar.categoryCount; ++i)
    {
        addCategory(&categories, GRAMMAR_XML.common.grammar.category[i]);
    }

    if (&grammar != &GRAMMAR_XML.common.grammar) {
        // add language specific categories
        for (int32_t i = 0; i < grammar.categoryCount; ++i)
        {
            addCategory(&categories, grammar.category[i]);
        }
    }

    return categories;
}

int32_t LanguageGrammarFeatures::getFeaturesCount() const
{
    return features.featuresCount;
}

std::set<LanguageGrammarFeatures_GrammarFeatures> LanguageGrammarFeatures::getFeatures() const
{
    std::set<LanguageGrammarFeatures_GrammarFeatures> result;

    // add all features from common
    for (int32_t i = 0; i < GRAMMAR_XML.common.features.featuresCount; ++i) {
        result.emplace(LanguageGrammarFeatures_GrammarFeatures(
                GRAMMAR_XML.common.features.feature[i].name,
                std::vector<LanguageGrammarFeatures_Feature>()));
    }

    if (&grammar != &GRAMMAR_XML.common.grammar) {
        // add all features for the current language
        for (int32_t i = 0; i < features.featuresCount; ++i) {
            std::vector<LanguageGrammarFeatures_Feature> values;
            const auto &feature = features.feature[i];
            for (int32_t j = 0; j < feature.valuesCount; ++j) {
                std::map<std::u16string, std::u16string> constraints;

                const auto &value = feature.value[j];
                if (value.number != nullptr) {
                    constraints[u"number"] = value.number;
                }
                if (value.gender != nullptr) {
                    constraints[u"gender"] = value.gender;
                }
                if (value.sound != nullptr) {
                    constraints[u"sound"] = value.sound;
                }

                values.emplace_back(value.result, constraints);
            }
            result.emplace(LanguageGrammarFeatures_GrammarFeatures(feature.name, values));
        }
    }

    return result;
}

} // namespace inflection::lang::features
