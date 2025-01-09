/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_EmptyConstraintInflectorPattern.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_EmptyConstraintInflectorPattern::NlGrammarSynthesizer_EmptyConstraintInflectorPattern()
    : super()
{
}

bool NlGrammarSynthesizer_EmptyConstraintInflectorPattern::isSupported(const ::std::u16string& /*displayString*/, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    return constraints.empty();
}

::std::u16string NlGrammarSynthesizer_EmptyConstraintInflectorPattern::inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (isSupported(displayString, constraints)) {
        return displayString;
    } else {
        return {};
    }
}

} // namespace inflection::grammar::synthesis
