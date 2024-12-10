/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/analysis/RussianExposableMorphology.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <unicode/uchar.h>
#include <algorithm>
#include <iterator>

using morphuntion::grammar::synthesis::GrammemeConstants;

namespace morphuntion::analysis {

RussianExposableMorphology::RussianExposableMorphology()
    : ::morphuntion::dialog::DictionaryLookupInflector(::morphuntion::util::LocaleUtils::RUSSIAN(), {
        {GrammemeConstants::POS_PROPER_NOUN(), GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
        {GrammemeConstants::CASE_NOMINATIVE(), GrammemeConstants::CASE_GENITIVE(), GrammemeConstants::CASE_DATIVE(), GrammemeConstants::CASE_ACCUSATIVE(), GrammemeConstants::CASE_INSTRUMENTAL(), GrammemeConstants::CASE_PREPOSITIONAL()},
        {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
        {GrammemeConstants::ANIMACY_INANIMATE(), GrammemeConstants::ANIMACY_ANIMATE()},
        {GrammemeConstants::DERIVATIONTYPE_PATRONYMIC()},
        {GrammemeConstants::REGISTER_INFORMAL()}
    }, {{GrammemeConstants::POS_PROPER_NOUN(), GrammemeConstants::NUMBER_PLURAL()}}, true)
{

    const auto &dictionary = getDictionary();
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun,{u"noun"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective,{u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdverb,{u"adverb"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryVerb,{u"verb"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNumber,{u"number"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryProperNoun,{u"proper-noun"}));
    dictionaryPOSMask = dictionaryNoun | dictionaryAdjective | dictionaryNumber | dictionaryProperNoun | dictionaryVerb | dictionaryAdverb;

    inflectableChars.addAll(morphuntion::lang::StringFilterUtil::getExemplarCharacters(::morphuntion::util::LocaleUtils::RUSSIAN()));
    inflectableChars.freeze();
}

RussianExposableMorphology::~RussianExposableMorphology() = default;

//Changed it so that even if it's something else(for eg: verb), what matters is that we have the noun as a POS property
bool RussianExposableMorphology::isNoun(int64_t phraseType) const {
    return (phraseType & dictionaryNoun) == dictionaryNoun;
}

bool RussianExposableMorphology::isProperNoun(int64_t phraseType) const {
    return (phraseType & dictionaryProperNoun) == dictionaryProperNoun;
}

bool RussianExposableMorphology::isAdjective(int64_t phraseType) const {
    return (phraseType & dictionaryPOSMask) == dictionaryAdjective;
}

bool RussianExposableMorphology::isNumber(int64_t phraseType) const {
    return (phraseType & dictionaryPOSMask) == dictionaryNumber;
}

bool RussianExposableMorphology::isInflectable(std::u16string_view word) const {
    return (word.length() > 2
        && inflectableChars.containsAllCodePoints(word)
        && doNotInflect.count(word) == 0);
}

::std::optional<::std::u16string> RussianExposableMorphology::inflectUsingDictionary(const ::std::u16string &word, const ::std::u16string &caseString, const ::std::u16string &countString, const ::std::u16string &genderString, const ::std::u16string &animacyString, const ::std::u16string &posString) const{
    const auto &dictionary = getDictionary();
    if (!isInflectable(word)) {
        return {word};
    }

    int64_t phraseType = 0;
    dictionary.getCombinedBinaryType(&phraseType, word);

    //Word not found in dictionary
    if (phraseType == 0) {
        return {};
    }

    //Word with first letter uppercase but not found in dictionary as noun/proper-noun
    auto firstCodePoint = morphuntion::util::StringViewUtils::codePointAt(word, 0);
    bool isInitCaps = firstCodePoint == (char32_t)u_toupper(firstCodePoint);
    if (isInitCaps && !isProperNoun(phraseType) && !isNoun(phraseType)) {
        return {};
    }

    bool mlCanInflect = isNoun(phraseType) || isNumber(phraseType) || isAdjective(phraseType) || (posString == u"proper-noun");

    ::std::u16string lowerCasedWord;
    morphuntion::util::StringViewUtils::lowercase(&lowerCasedWord, word, ::morphuntion::util::LocaleUtils::RUSSIAN());
    auto inflectionResult = inflectWord(lowerCasedWord, {caseString, countString, genderString, animacyString});
    ::std::u16string inflectedWord;

    if (!inflectionResult.has_value()) {
        //Dictionary can't inflect but ML can
        if (mlCanInflect) {
            return {};
        }
        inflectedWord = word;
    } else {
        inflectedWord = *inflectionResult;
    }

    inflectedWord[0] = word[0]; // Keep the letter case of the first letter.
    // If Word all uppercase -> Make inflection all upper case
    if (morphuntion::util::StringViewUtils::isAllUpperCase(word)) {
        ::std::u16string upperCaseInflectedWord;
        morphuntion::util::StringViewUtils::uppercase(&upperCaseInflectedWord, inflectedWord, ::morphuntion::util::LocaleUtils::RUSSIAN());
        inflectedWord = upperCaseInflectedWord;
    }
    return {inflectedWord};
}

} // namespace morphuntion::analysis
