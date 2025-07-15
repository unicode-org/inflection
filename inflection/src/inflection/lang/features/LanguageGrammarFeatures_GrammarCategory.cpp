/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>

namespace inflection::lang::features {

LanguageGrammarFeatures_GrammarCategory::LanguageGrammarFeatures_GrammarCategory(const std::u16string& name,
                                                                                 const std::set<std::u16string>& values,
                                                                                 const std::multimap<std::u16string, std::u16string>& grammemeDependenciesForCategory,
                                                                                 const std::map<std::u16string_view, std::multimap<std::u16string, std::u16string>>& grammemeDependenciesForValues)
    : super()
    , name(name)
    , values(values)
    , grammemeDependenciesForCategory(grammemeDependenciesForCategory)
    , grammemeDependenciesForValues(grammemeDependenciesForValues)
{
}

LanguageGrammarFeatures_GrammarCategory::~LanguageGrammarFeatures_GrammarCategory()
{
}

std::u16string LanguageGrammarFeatures_GrammarCategory::getName() const
{
    return name;
}

std::set<std::u16string> LanguageGrammarFeatures_GrammarCategory::getValues() const
{
    return values;
}

bool LanguageGrammarFeatures_GrammarCategory::isUniqueValues() const
{
    return uniqueValues;
}

std::multimap<std::u16string, std::u16string> LanguageGrammarFeatures_GrammarCategory::getGrammemeDependenciesForCategory() const
{
    return grammemeDependenciesForCategory;
}

std::multimap<std::u16string, std::u16string> LanguageGrammarFeatures_GrammarCategory::getGrammemeDependenciesForValue(std::u16string_view grammemeValue) const
{
    auto result = grammemeDependenciesForValues.find(grammemeValue);
    if (result != grammemeDependenciesForValues.end()) {
        return result->second;
    }
    return {};
}

std::weak_ordering LanguageGrammarFeatures_GrammarCategory::operator<=>(const LanguageGrammarFeatures_GrammarCategory& other) const
{
    return name <=> other.name;
}

} // namespace inflection::lang::features
