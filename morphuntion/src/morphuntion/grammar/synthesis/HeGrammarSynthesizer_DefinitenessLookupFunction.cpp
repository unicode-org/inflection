/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::grammar::synthesis {

HeGrammarSynthesizer_DefinitenessLookupFunction::HeGrammarSynthesizer_DefinitenessLookupFunction()
    : super(::morphuntion::util::LocaleUtils::HEBREW(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE(), GrammemeConstants::DEFINITENESS_CONSTRUCT()})
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
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
    if (::morphuntion::util::StringViewUtils::startsWith(word, u"ת") || ::morphuntion::util::StringViewUtils::startsWith(word, u"ה")) {
        // Definite unknown word.
        return GrammemeConstants::DEFINITENESS_DEFINITE();
    }
    return ::std::u16string();
}

} // namespace morphuntion::grammar::synthesis
