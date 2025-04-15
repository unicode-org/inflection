/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <inflection/util/LocaleUtils.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::grammar::synthesis {

HeGrammarSynthesizer_DefinitenessLookupFunction::HeGrammarSynthesizer_DefinitenessLookupFunction()
    : super(::inflection::util::LocaleUtils::HEBREW(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE(), GrammemeConstants::DEFINITENESS_CONSTRUCT()})
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_PROPER_NOUN()}));
}

::std::u16string HeGrammarSynthesizer_DefinitenessLookupFunction::determine(const ::std::u16string& word) const
{
    auto out(super::determine(word));
    if (!out.empty()) {
        return out;
    }

    int64_t wordProperties = 0;
    if (dictionary.getCombinedBinaryType(&wordProperties, word) != nullptr) {
        if ((wordProperties & nounProperty) != 0) {
            // Nouns are indefinite in the dictionary.
            return GrammemeConstants::DEFINITENESS_INDEFINITE();
        }
        // Else it's a known word that isn't a noun. Definite forms are not in the dictionary.
        return ::std::u16string();
    }
    // Unknown word
    if (word.starts_with(u"ת") || word.starts_with(u"ה")) {
        // Definite unknown word.
        return GrammemeConstants::DEFINITENESS_DEFINITE();
    }
    return ::std::u16string();
}

} // namespace inflection::grammar::synthesis
