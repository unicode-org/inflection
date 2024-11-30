/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>

namespace morphuntion::lang::features {

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

int32_t LanguageGrammarFeatures_GrammarCategory::compareTo(const LanguageGrammarFeatures_GrammarCategory& o) const
{
    return name.compare(o.name);
}

bool LanguageGrammarFeatures_GrammarCategory::operator<(const LanguageGrammarFeatures_GrammarCategory& other) const
{
    return (name.compare(other.name)) < 0;
}

} // namespace morphuntion::lang::features
