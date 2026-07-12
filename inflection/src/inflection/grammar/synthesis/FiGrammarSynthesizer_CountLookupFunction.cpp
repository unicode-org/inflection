/*
 * Copyright 2019-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_CountLookupFunction.hpp>

#include <inflection/grammar/synthesis/FiGrammarSynthesizer_FiDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

FiGrammarSynthesizer_CountLookupFunction::FiGrammarSynthesizer_CountLookupFunction(const FiGrammarSynthesizer_FiDisplayFunction& displayFunction)
    : super(::inflection::util::LocaleUtils::FINNISH(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}, {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE})
    , displayFunction(displayFunction)
{
}

::std::u16string FiGrammarSynthesizer_CountLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }
    auto exemplar = displayFunction.getExemplar(word);
    if (exemplar.has_value()) {
        out = super::determine(*exemplar);
        if (!out.empty()) {
            return out;
        }
    }
    return GrammemeConstants::NUMBER_SINGULAR;
}

} // namespace inflection::grammar::synthesis
