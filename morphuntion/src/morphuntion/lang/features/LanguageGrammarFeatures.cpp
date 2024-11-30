/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>

#include <morphuntion/lang/features/LanguageGrammarFeatures_Data.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_Feature.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarFeatures.hpp>
#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/resources/DataResource.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::lang::features {

LanguageGrammarFeatures::LanguageGrammarFeatures(const ::morphuntion::util::ULocale& pLanguage, const Grammar& grammar, const Features& features)
    : super()
    , language(pLanguage)
    , grammar(grammar)
    , features(features)
{
}

LanguageGrammarFeatures::~LanguageGrammarFeatures()
{
}

LanguageGrammarFeatures LanguageGrammarFeatures::getLanguageGrammarFeatures(const ::morphuntion::util::ULocale& locale)
{
    if (locale.getName().empty()) {
        return LanguageGrammarFeatures(locale, GRAMMAR_XML.common.grammar, GRAMMAR_XML.common.features);
    }
    std::u16string languageStr;
    morphuntion::util::StringViewUtils::convert(&languageStr, locale.getLanguage());
    for (int32_t i = 0; i < GRAMMAR_XML.languageCount; ++i) {
        if (languageStr == GRAMMAR_XML.languages[i].id) {
            return LanguageGrammarFeatures(locale, GRAMMAR_XML.languages[i].grammar, GRAMMAR_XML.languages[i].features);
        }
    }

    throw ::morphuntion::exception::IllegalArgumentException(u"Language features can not be found for language " + locale.toString());
}

std::set<::morphuntion::util::ULocale> LanguageGrammarFeatures::getKnownLanguages()
{
    std::set<::morphuntion::util::ULocale> languages;
    for (int32_t i = 0; i < GRAMMAR_XML.languageCount; ++i) {
        languages.emplace(morphuntion::util::StringViewUtils::to_string(GRAMMAR_XML.languages[i].id));
    }
    languages.emplace(morphuntion::util::LocaleUtils::ROOT()); // common

    return languages;
}

morphuntion::util::ULocale LanguageGrammarFeatures::getLanguage() const
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

} // namespace morphuntion::lang::features
