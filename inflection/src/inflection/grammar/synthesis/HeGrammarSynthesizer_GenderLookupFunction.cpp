/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_GenderLookupFunction.hpp>

#include <inflection/util/LocaleUtils.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::grammar::synthesis {

HeGrammarSynthesizer_GenderLookupFunction::HeGrammarSynthesizer_GenderLookupFunction()
    : super(::inflection::util::LocaleUtils::HEBREW(), {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE()})
{
}

::std::u16string HeGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }

    if (word.ends_with(u"ת") || word.ends_with(u"ה")) {
        return GrammemeConstants::GENDER_FEMININE();
    }
    return GrammemeConstants::GENDER_MASCULINE();
}

} // namespace inflection::grammar::synthesis
