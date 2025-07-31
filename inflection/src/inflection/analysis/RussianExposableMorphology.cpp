/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/analysis/RussianExposableMorphology.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <unicode/uchar.h>
#include <algorithm>
#include <iterator>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::analysis {

RussianExposableMorphology::RussianExposableMorphology()
    : ::inflection::dialog::DictionaryLookupInflector(::inflection::util::LocaleUtils::RUSSIAN(), {
        {GrammemeConstants::POS_PROPER_NOUN(), GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
        {GrammemeConstants::CASE_NOMINATIVE(), GrammemeConstants::CASE_GENITIVE(), GrammemeConstants::CASE_DATIVE(), GrammemeConstants::CASE_ACCUSATIVE(), GrammemeConstants::CASE_INSTRUMENTAL(), GrammemeConstants::CASE_PREPOSITIONAL()},
        {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
        {GrammemeConstants::ANIMACY_INANIMATE(), GrammemeConstants::ANIMACY_ANIMATE()},
        {GrammemeConstants::REGISTER_INFORMAL()}
    }, {{GrammemeConstants::POS_PROPER_NOUN(), GrammemeConstants::NUMBER_PLURAL()}}, true)
{
    const auto &dictionary = getDictionary();
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {GrammemeConstants::POS_NOUN()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdverb, {u"adverb"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryVerb, {GrammemeConstants::POS_VERB()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNumber, {u"numeral"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryProperNoun, {GrammemeConstants::POS_PROPER_NOUN()}));
    dictionaryPOSMask = dictionaryNoun | dictionaryAdjective | dictionaryNumber | dictionaryProperNoun | dictionaryVerb | dictionaryAdverb;

    inflectableChars.addAll(inflection::lang::StringFilterUtil::getExemplarCharacters(::inflection::util::LocaleUtils::RUSSIAN()));
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

bool RussianExposableMorphology::isInflectableByMachineLearning(int64_t phraseType) const {
    return (phraseType & dictionaryNoun) == dictionaryNoun
        || (phraseType & dictionaryPOSMask) == dictionaryNumber
        || (phraseType & dictionaryPOSMask) == dictionaryAdjective;
}

bool RussianExposableMorphology::isInflectable(std::u16string_view word) const {
    return (word.length() > 2
        && inflectableChars.containsAllCodePoints(word)
        && !doNotInflect.contains(word));
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
    auto firstCodePoint = inflection::util::StringViewUtils::codePointAt(word, 0);
    bool isInitCaps = firstCodePoint == (char32_t)u_toupper((UChar32)firstCodePoint);
    if (isInitCaps && !isProperNoun(phraseType) && !isNoun(phraseType)) {
        return {};
    }

    bool mlCanInflect = isInflectableByMachineLearning(phraseType) || (posString == u"proper-noun");

    ::std::u16string lowerCasedWord;
    inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, ::inflection::util::LocaleUtils::RUSSIAN());
    auto inflectionResult = inflectWord(lowerCasedWord, phraseType, {caseString, countString, genderString, animacyString});
    ::std::u16string inflectedWord;

    if (!inflectionResult.has_value()) {
        // Dictionary can't inflect but ML can
        if (mlCanInflect) {
            return {};
        }
        inflectedWord = word;
    } else {
        inflectedWord = *inflectionResult;
    }

    inflectedWord[0] = word[0]; // Keep the letter case of the first letter.
    // If Word all uppercase -> Make inflection all upper case
    if (inflection::util::StringViewUtils::isAllUpperCase(word)) {
        ::std::u16string upperCaseInflectedWord;
        inflection::util::StringViewUtils::uppercase(&upperCaseInflectedWord, inflectedWord, ::inflection::util::LocaleUtils::RUSSIAN());
        inflectedWord = upperCaseInflectedWord;
    }
    return {inflectedWord};
}

} // namespace inflection::analysis
