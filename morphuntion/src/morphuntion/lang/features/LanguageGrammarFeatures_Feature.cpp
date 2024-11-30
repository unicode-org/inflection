/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/lang/features/LanguageGrammarFeatures_Feature.hpp>

namespace morphuntion::lang::features {


LanguageGrammarFeatures_Feature::LanguageGrammarFeatures_Feature(const std::u16string& value, const std::map<std::u16string, std::u16string>& constraints)
    : super()
    , value(value)
    , constraints(constraints)
{
}

LanguageGrammarFeatures_Feature::~LanguageGrammarFeatures_Feature()
{
}

std::u16string LanguageGrammarFeatures_Feature::getValue() const
{
    return value;
}

std::map<std::u16string, std::u16string> LanguageGrammarFeatures_Feature::getConstraints() const
{
    return constraints;
}

} // namespace morphuntion::lang::features
