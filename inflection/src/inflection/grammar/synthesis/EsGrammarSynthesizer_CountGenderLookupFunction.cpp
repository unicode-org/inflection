/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_CountGenderLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

EsGrammarSynthesizer_CountGenderLookupFunction::EsGrammarSynthesizer_CountGenderLookupFunction(GrammarCategoryType categoryType, const ::std::vector<::std::u16string>& properties)
    : super(::inflection::util::LocaleUtils::SPANISH(), properties, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE()})
    , dictionary(getDictionary())
    , grammarCategoryType(categoryType)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {GrammemeConstants::POS_NOUN()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&propertiesMask, properties));
}

EsGrammarSynthesizer_CountGenderLookupFunction::~EsGrammarSynthesizer_CountGenderLookupFunction()
{
}

::std::u16string guessGender(::std::u16string_view word, bool knownWord) {
    for (const ::std::u16string_view ending: {u"is", u"iones", u"ie", u"ción", u"sión", u"umbre"}) {
        if (word.ends_with(ending)) {
            return GrammemeConstants::GENDER_FEMININE();
        }
    }
    // There is a strong bias in the words ending with -a and -as to be feminine
    if (!knownWord) {
        for (const ::std::u16string_view ending: {u"as", u"a"}) {
            if (word.ends_with(ending)) {
                return GrammemeConstants::GENDER_FEMININE();
            }
        }
    }
    return GrammemeConstants::GENDER_MASCULINE();
}

::std::u16string guessCount(::std::u16string_view word) {
    for (const ::std::u16string_view ending: {u"ís", u"és", u"ás"}) {
        if (word.ends_with(ending)) {
            return GrammemeConstants::NUMBER_SINGULAR();
        }
    }
    if (word.ends_with(u"s")) {
        return GrammemeConstants::NUMBER_PLURAL();
    }
    return GrammemeConstants::NUMBER_SINGULAR();
}

::std::u16string EsGrammarSynthesizer_CountGenderLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determinePhrase(word, true);
    if (out.empty() && !word.empty()) {
        int64_t wordGrammemes = 0;
        bool knownWord = dictionary.getCombinedBinaryType(&wordGrammemes, word) != nullptr;
        // Known word doesn't have a gender, return empty
        if (grammarCategoryType == GENDER_CATEGORY && knownWord && ((wordGrammemes & propertiesMask) == 0)) {
            return {};
        }
        // Otherwise guess gender/count with the first token
        std::u16string lowercasedWord;
        inflection::util::StringViewUtils::lowercase(&lowercasedWord, getFirstWord(word), inflection::util::LocaleUtils::SPANISH());
        if (grammarCategoryType == GENDER_CATEGORY) {
            int64_t stringTokenGrammemes = 0;
            bool knownStringToken = dictionary.getCombinedBinaryType(&stringTokenGrammemes, lowercasedWord);
            out = guessGender(lowercasedWord, knownStringToken);
        }
        else if (grammarCategoryType == NUMBER_CATEGORY) {
            out = guessCount(lowercasedWord);
        }
    }
    return out;
}

} // namespace inflection::grammar::synthesis
