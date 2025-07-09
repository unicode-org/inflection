/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarFeatures.hpp>

namespace inflection::lang::features {

LanguageGrammarFeatures_GrammarFeatures::LanguageGrammarFeatures_GrammarFeatures(const std::u16string& name, const std::vector<LanguageGrammarFeatures_Feature>& values)
    : super()
    , name(name)
    , values(values)
{
}

LanguageGrammarFeatures_GrammarFeatures::~LanguageGrammarFeatures_GrammarFeatures()
{
}

std::u16string LanguageGrammarFeatures_GrammarFeatures::getName() const
{
    return name;
}

std::vector<LanguageGrammarFeatures_Feature> LanguageGrammarFeatures_GrammarFeatures::getValues() const
{
    return values;
}

std::weak_ordering LanguageGrammarFeatures_GrammarFeatures::operator<=>(const LanguageGrammarFeatures_GrammarFeatures& other) const
{
    return name <=> other.name;
}

} // namespace inflection::lang::features
