/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_HiDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/PronounConcept.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <algorithm>
#include <vector>

namespace inflection::grammar::synthesis {

using inflection::dialog::SemanticFeature;
using inflection::dialog::SemanticFeatureModel;
using inflection::dialog::DisplayValue;
using inflection::tokenizer::TokenChain;
using inflection::util::LocaleUtils;
using inflection::util::Validate;
using inflection::tokenizer::TokenizerFactory;

HiGrammarSynthesizer_HiDisplayFunction::HiGrammarSynthesizer_HiDisplayFunction(const SemanticFeatureModel& model)
    : super()
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , tokenizer(TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::HINDI()))
    , dictionaryInflector(LocaleUtils::HINDI(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADPOSITION(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::VERBTYPE_PARTICIPLE()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_SECOND(), GrammemeConstants::PERSON_FIRST()},
            {GrammemeConstants::CASE_DIRECT(),  GrammemeConstants::CASE_OBLIQUE()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    }, {}, true)
{
    Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&pluralVerbMask, {GrammemeConstants::NUMBER_PLURAL(), GrammemeConstants::POS_VERB()}));
    Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&pluralFeminineMask, {GrammemeConstants::NUMBER_PLURAL(), GrammemeConstants::GENDER_FEMININE()}));
    Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&genderMask, {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}));
    Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&adpositionMask, {GrammemeConstants::POS_ADPOSITION()}));
}

HiGrammarSynthesizer_HiDisplayFunction::~HiGrammarSynthesizer_HiDisplayFunction() {}

namespace {
    ::std::u16string guessSingularInflection(const ::std::u16string& word) {
        // बटुए -> बटुआ
        if (word.ends_with(u"ए")) {
            return word.substr(0, word.length() - 1) + u"आ";
        }

        // लड़के -> लड़का
        if (word.ends_with(u"े")) {
            return word.substr(0, word.length() - 1) + u"ा";
        }

        // किताबें -> किताब
        if (word.ends_with(u"ें")) {
            return word.substr(0, word.length() - 1);
        }
        return word;
    }

    ::std::u16string guessPluralInflection(const ::std::u16string& word) {
        // Case 1
        // लड़की -> लड़कियाँ, नीति -> नीतियाँ
        if (word.ends_with(u"ी") || word.ends_with(u"ि")) {
            return word.substr(0, word.length() - 1) + u"ियाँ";
        }

        // Case 2
        // वस्तु -> वस्तुएँ
        if (word.ends_with(u"ू") || word.ends_with(u"ु")) {
            return word.substr(0, word.length() - 1) + u"ुएँ";
        }

        // Case 3: This should always come before Case 5
        // चिड़िया -> चिड़ियाँ
        if (word.ends_with(u"या")) {
            return word.substr(0, word.length() - 1) + u"ाँ";
        }

        // Case 4
        // बटुआ -> बटुए
        if (word.ends_with(u"आ")) {
            return word.substr(0, word.length() - 1) + u"ए";
        }

        // Case 5: Comes after Case 3
        // लड़का -> लड़के
        if (word.ends_with(u"ा")) {
            return word.substr(0, word.length() - 1) + u"े";
        }
        return word;
    }

    ::std::u16string guessMasculineInflection(const ::std::u16string& word) {
        // लड़की -> लड़का
        if (word.ends_with(u"ी")) {
            return word.substr(0, word.length() - 1) + u"ा";
        }
        return word;
    }

    ::std::u16string guessFeminineInflection(const ::std::u16string& word) {
        // लड़का -> लड़की
        if (word.ends_with(u"ा")) {
            return word.substr(0, word.length() - 1) + u"ी";
        }
        return word;
    }

    bool isPluralVerb(int64_t propertiesMask, int64_t pluralVerbMask) {
        return (propertiesMask & pluralVerbMask) == pluralVerbMask;
    }

    bool isPluralFeminine(int64_t propertiesMask, int64_t pluralFeminineMask) {
        return (propertiesMask & pluralFeminineMask) == pluralFeminineMask;
    }

    bool isAdposition(int64_t propertiesMask, int64_t adpositionMask) {
        return (propertiesMask & adpositionMask) == adpositionMask;
    }

    bool isInflectableAdposition(const std::u16string_view &candidateAdposition) {
        constexpr std::u16string_view inflectableAdpositions[] = {u"का", u"के", u"की"};
        return std::find(inflectableAdpositions, inflectableAdpositions + 3, candidateAdposition) != (inflectableAdpositions + 3);
    }
}

::std::optional<::std::u16string> HiGrammarSynthesizer_HiDisplayFunction::inflectWord(const ::std::u16string& word, int64_t wordProperties, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess, bool makeOblique) const {
    ::std::vector<::std::u16string> constraintsVec;
    if (const auto constraintString = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature); !constraintString.empty()) {
        constraintsVec.emplace_back(constraintString);
    }
    if ((wordProperties & genderMask) != 0 && !makeOblique) {
        if (const auto constraintString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature); !constraintString.empty()) {
            constraintsVec.emplace_back(constraintString);
        }
    }
    if (const auto constraintString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature); !constraintString.empty()) {
        constraintsVec.emplace_back(constraintString);
    }

    if (makeOblique) {
        constraintsVec.emplace_back(GrammemeConstants::CASE_OBLIQUE());
    }
    const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
    if (const auto &inflectionResult = dictionaryInflector.inflect(word, wordProperties, constraintsVec, disambiguationGrammemeValues)) {
        return *inflectionResult;
    }

    if (!enableInflectionGuess) {
        return std::nullopt;
    }

    // Well, dictionary was not able to inflect it. So let's make a guess.
    auto inflectedWord(word);
    const auto gender(GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
    if (gender == GrammemeConstants::GENDER_FEMININE()) {
        inflectedWord = guessFeminineInflection(inflectedWord);
    }
    else if (gender == GrammemeConstants::GENDER_MASCULINE()) {
        inflectedWord = guessMasculineInflection(inflectedWord);
    }
    const auto count(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    if (count == GrammemeConstants::NUMBER_PLURAL()) {
        inflectedWord = guessPluralInflection(inflectedWord);
    }
    else if (count == GrammemeConstants::NUMBER_SINGULAR()) {
        inflectedWord = guessSingularInflection(inflectedWord);
    }
    return inflectedWord;
}

::std::optional<::std::vector<::std::u16string>> HiGrammarSynthesizer_HiDisplayFunction::inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const {
    if (words.empty()) {
        return {{}};
    }
    const auto &dictionary = dictionaryInflector.getDictionary();
    int64_t adpositionIndex = -1;

    int32_t idx = 0;
    for (const auto &word: words) {
        int64_t wordProperties = 0;
        dictionary.getCombinedBinaryType(&wordProperties, word);
        if (isAdposition(wordProperties, adpositionMask)) {
            adpositionIndex = idx;
            break;
        }
        idx += 1;
    }

    std::vector<std::u16string> inflectedWords;
    inflectedWords.reserve(words.size());
    idx = 0;
    for (const auto &word: words) {
        const bool processingAdposition = (idx == adpositionIndex);
        // If an adposition is present in the phrase then inflect all words
        // before the adposition to oblique
        // Eg: दरवाज़ा में becomes दरवाजे में
        // Eg: दरवाज़ा का becomes दरवाज़े का
        const bool makeOblique = idx < adpositionIndex;
        idx += 1;
        // If we are processing the adposition but it does not inflect then don't perform inflect
        if (const bool inflectionNotRequired = processingAdposition && !isInflectableAdposition(word); inflectionNotRequired) {
            inflectedWords.emplace_back(word);
            continue;
        }

        int64_t wordProperties = 0;
        dictionaryInflector.getDictionary().getCombinedBinaryType(&wordProperties, word);
        if (const auto &inflectionResult = inflectWord(word, wordProperties, constraints, enableInflectionGuess, makeOblique); inflectionResult) {
            inflectedWords.emplace_back(*inflectionResult);
        } else if (enableInflectionGuess) {
            inflectedWords.emplace_back(word);
        } else {
            return std::nullopt;
        }
    }

    if (words.size() == 1) {
        return inflectedWords;
    }

    // Implementing the rule where if the last word is a plural verb then all the preceding feminine words are inflected to singular
    // For eg: plural of चाहती is चाहतीं but when followed by हैं it becomes चाहती हैं
    const auto &lastInflectedWord = inflectedWords.back();
    int64_t lastInflectedWordProperties = 0;
    dictionary.getCombinedBinaryType(&lastInflectedWordProperties, lastInflectedWord);
    if (isPluralVerb(lastInflectedWordProperties, pluralVerbMask)) {
        for (int32_t i = 0; i < int32_t(inflectedWords.size() - 1); i += 1) {
            const auto &inflectedWord = inflectedWords.at(i);
            int64_t inflectedWordProperties = 0;
            dictionary.getCombinedBinaryType(&inflectedWordProperties, inflectedWord);
            if (!isPluralFeminine(inflectedWordProperties, pluralFeminineMask)) {
                continue;
            }

            if (const auto& singularInflectedWord = dictionaryInflector.inflect(inflectedWord, inflectedWordProperties, {GrammemeConstants::NUMBER_SINGULAR()})) {
                inflectedWords.at(i) = *singularInflectedWord;
            } else if (enableInflectionGuess) {
                inflectedWords.at(i) = guessSingularInflection(inflectedWords.at(i));
            } else {
                return std::nullopt;
            }
        }
    }
    return inflectedWords;
}

::std::optional<::std::u16string> HiGrammarSynthesizer_HiDisplayFunction::inflectTokenChain(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const {
    const auto &inflectedSignificantWordsResult = inflectSignificantWords(GrammarSynthesizerUtil::getSignificantWords(tokenChain), constraints, enableInflectionGuess);
    if (!inflectedSignificantWordsResult) {
        return std::nullopt;
    }
    return GrammarSynthesizerUtil::getStringFromInflectedSignificantWords(tokenChain, *inflectedSignificantWordsResult);
}


DisplayValue* HiGrammarSynthesizer_HiDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const {
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    auto displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    const auto displayValueConstraints(GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (!constraints.empty()) {
        ::std::optional<::std::u16string> inflectionResult;
        int64_t wordProperties = 0;
        if (dictionaryInflector.getDictionary().getCombinedBinaryType(&wordProperties, displayString) != nullptr) {
            inflectionResult = inflectWord(displayString, wordProperties, constraints, enableInflectionGuess, false);
        }
        else {
            const ::std::unique_ptr<TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            inflectionResult = inflectTokenChain(*npc(tokenChain.get()), constraints, enableInflectionGuess);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess) {
            return nullptr;
        }
    }
    return new DisplayValue(displayString, displayValueConstraints);
}

} // namespace inflection::grammar::synthesis
