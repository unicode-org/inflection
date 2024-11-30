/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_GenderLookupFunction.hpp>

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::grammar::synthesis {

HeGrammarSynthesizer_GenderLookupFunction::HeGrammarSynthesizer_GenderLookupFunction()
    : super(::morphuntion::util::LocaleUtils::HEBREW(), {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE()})
{
}

::std::u16string HeGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }

    if (::morphuntion::util::StringViewUtils::endsWith(word, u"ת") || ::morphuntion::util::StringViewUtils::endsWith(word, u"ה")) {
        return GrammemeConstants::GENDER_FEMININE();
    }
    return GrammemeConstants::GENDER_MASCULINE();
}

} // namespace morphuntion::grammar::synthesis
