/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_RuDisplayFunction.hpp>

#include <inflection/tokenizer/Token.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>
#include <memory>

// derived from genExemplars when generating suffix_ru.csv
static constexpr int32_t MAX_SUFFIX_LEN = 5;

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_RuDisplayFunction::RuGrammarSynthesizer_RuDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::RUSSIAN())))
    , russianMorphology(*npc(::inflection::analysis::RussianExposableMorphology::getInstance()))
    , suffixToExemplar(inflection::util::ResourceLocator::getRootForLocale(inflection::util::LocaleUtils::RUSSIAN()) + u"/exemplar/suffix_" + inflection::util::LocaleUtils::RUSSIAN().toString() + u".bist")
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , animacyFeature(*npc(model.getFeature(GrammemeConstants::ANIMACY)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
{
    const auto& dictionary = russianMorphology.getDictionary();
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAnimate, {GrammemeConstants::ANIMACY_ANIMATE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryInanimate, {GrammemeConstants::ANIMACY_INANIMATE}));
    dictionaryAnimacyMask = dictionaryAnimate | dictionaryInanimate;

    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {GrammemeConstants::GENDER_MASCULINE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {GrammemeConstants::GENDER_FEMININE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {GrammemeConstants::GENDER_NEUTER}));
    dictionaryGenderMask = dictionaryMasculine | dictionaryFeminine | dictionaryNeuter;

    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL}));
    dictionaryNumberMask = dictionarySingular | dictionaryPlural;

    nonRussianCyrillicChars.removeAll(inflection::lang::StringFilterUtil::getExemplarCharacters(::inflection::util::LocaleUtils::RUSSIAN()));
    nonRussianCyrillicChars.freeze();
}

RuGrammarSynthesizer_RuDisplayFunction::~RuGrammarSynthesizer_RuDisplayFunction()
{
}

std::u16string RuGrammarSynthesizer_RuDisplayFunction::getAnimacy(int64_t grammemes) const {
    if ((grammemes & dictionaryAnimacyMask) == dictionaryAnimate) {
        return GrammemeConstants::ANIMACY_ANIMATE;
    }
    if ((grammemes & dictionaryAnimacyMask) == dictionaryInanimate) {
        return GrammemeConstants::ANIMACY_INANIMATE;
    }
    return {};
}

std::u16string RuGrammarSynthesizer_RuDisplayFunction::getGender(int64_t grammemes) const {
    grammemes = (grammemes & dictionaryGenderMask);
    if (grammemes == dictionaryMasculine) {
        return GrammemeConstants::GENDER_MASCULINE;
    }
    if (grammemes == dictionaryFeminine) {
        return GrammemeConstants::GENDER_FEMININE;
    }
    if (grammemes == dictionaryNeuter) {
        return GrammemeConstants::GENDER_NEUTER;
    }
    return {};
}

std::u16string RuGrammarSynthesizer_RuDisplayFunction::getNumber(int64_t grammemes) const {
    grammemes = (grammemes & dictionaryNumberMask);
    if (grammemes == dictionarySingular) {
        return GrammemeConstants::NUMBER_SINGULAR;
    }
    if (grammemes == dictionaryPlural) {
        return GrammemeConstants::NUMBER_PLURAL;
    }
    return {};
}

std::vector<std::u16string> RuGrammarSynthesizer_RuDisplayFunction::getConstraintsVector(const std::u16string& caseString, const std::u16string& countString, const std::u16string& genderString, const std::u16string& animacyString) {
    std::vector<std::u16string> constraintStrings;
    for (const auto &constraint : {caseString, countString, genderString, animacyString}) {
        if (!constraint.empty()) {
            constraintStrings.push_back(constraint);
        }
    }
    return constraintStrings;
}

std::optional<::std::u16string> RuGrammarSynthesizer_RuDisplayFunction::guessInflection(const ::std::u16string &word, const std::vector<std::u16string>& constraints, const ::std::u16string& posString) const
{
    if (word.length() <= 2 || !russianMorphology.isInflectable(word)) {
        return {};
    }

    for (int32_t suffixLen = std::min(MAX_SUFFIX_LEN, static_cast<int32_t>(word.length())); suffixLen > 0; --suffixLen) {
        std::u16string_view suffix = std::u16string_view(word).substr(word.length() - suffixLen);
        auto exemplarResult = suffixToExemplar.findTarget(suffix);
        if (exemplarResult.has_value()) {
            const auto& exemplar = *exemplarResult;
            int64_t similarGrammemes = 0;
            russianMorphology.getDictionary().getCombinedBinaryType(&similarGrammemes, exemplar);

            auto inflectionResult = inflectUsingDictionary(exemplar, similarGrammemes, constraints, posString);
            if (inflectionResult.has_value()) {
                std::u16string inflection = inflectionResult.value();
                const auto inflectionLength = static_cast<int32_t>(inflection.length());
                const auto exemplarLength = static_cast<int32_t>(exemplar.length());
                int32_t indexDiff;
                for (indexDiff = 0; indexDiff < inflectionLength && indexDiff < exemplarLength && inflection[indexDiff] == exemplar[indexDiff]; ++indexDiff) {
                }
                inflection.replace(0, indexDiff, word, 0, word.length() - (exemplarLength - indexDiff));
                return inflection;
            }
        }
    }

    return {};
}

static std::u16string remapCase(const std::u16string& caseString) {
    if (caseString == GrammemeConstants::CASE_ABLATIVE) {
        return GrammemeConstants::CASE_INSTRUMENTAL;
    }
    if (caseString == GrammemeConstants::CASE_LOCATIVE) {
        return GrammemeConstants::CASE_PREPOSITIONAL;
    }
    return caseString;
}

::std::optional<::std::u16string> RuGrammarSynthesizer_RuDisplayFunction::inflectUsingDictionary(const ::std::u16string &word, int64_t phraseType, const std::vector<std::u16string>& constraints, const ::std::u16string& posString) const {
    if (!russianMorphology.isInflectable(word)) {
        return {word};
    }

    if (phraseType == 0) {
        return {};
    }

    std::vector<std::u16string> optionalConstraints;
    if (!posString.empty()) {
        optionalConstraints.push_back(posString);
    }

    auto inflectionResult = russianMorphology.inflectWord(word, phraseType, constraints, optionalConstraints);
    if (inflectionResult.has_value()) {
        return inflectionResult.value();
    }

    ::std::u16string lowerCasedWord;
    inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, ::inflection::util::LocaleUtils::RUSSIAN());
    if (lowerCasedWord == word) {
        return {};
    }
    inflectionResult = russianMorphology.inflectWord(lowerCasedWord, phraseType, constraints, optionalConstraints);

    if (!inflectionResult.has_value()) {
        return {};
    }
    ::std::u16string inflectedWord(*inflectionResult);

    if (inflection::util::StringViewUtils::isAllUpperCase(word)) {
        ::std::u16string upperCaseInflectedWord;
        inflection::util::StringViewUtils::uppercase(&upperCaseInflectedWord, inflectedWord, ::inflection::util::LocaleUtils::RUSSIAN());
        inflectedWord = upperCaseInflectedWord;
    }
    else {
        inflectedWord[0] = word[0];
    }
    return {inflectedWord};
}

std::u16string RuGrammarSynthesizer_RuDisplayFunction::inflectPhrase(const inflection::tokenizer::TokenChain& words, const std::vector<std::u16string>& constraintStrings, bool enableInflectionGuess) const {
    std::u16string inflectionResult;
    auto begin = words.begin();
    for (auto wordIter = --words.end(); wordIter != begin; --wordIter)  {
        if (!wordIter->isSignificant()) {
            inflectionResult.insert(0, wordIter->getValue());
            continue;
        }
        auto word(wordIter->getValue());

        int64_t phraseType = 0;
        auto previous = wordIter->getPrevious();
        if (previous != nullptr && previous->getPrevious() != nullptr && previous->getValue() == u"-") {
            auto newWord(previous->getPrevious()->getValue() + previous->getValue() + word);
            if (russianMorphology.getDictionary().getCombinedBinaryType(&phraseType, newWord) != nullptr) {
                word = newWord;
                --wordIter;
                --wordIter;
            }
        }
        if (phraseType == 0) {
            russianMorphology.getDictionary().getCombinedBinaryType(&phraseType, word);
        }
        auto dictionaryResult(inflectUsingDictionary(word, phraseType, constraintStrings, std::u16string()));
        if (dictionaryResult) {
            inflectionResult.insert(0, *dictionaryResult);
        } else if (phraseType != 0) {
            inflectionResult.insert(0, word);
        } else if (enableInflectionGuess) {
            auto guessedResult = guessInflection(word, constraintStrings, std::u16string());
            inflectionResult.insert(0, guessedResult.value_or(word));
        } else {
            return {};
        }
    }
    return inflectionResult;
}

std::u16string RuGrammarSynthesizer_RuDisplayFunction::getStaticSuffix(const std::u16string &displayString, const std::vector<std::u16string>& constraintStrings, std::u16string& suffixResult) const {
    int64_t phraseType = 0;
    auto commaIdx = displayString.find(u',');
    if (commaIdx != ::std::string::npos) {
        suffixResult.assign(displayString, commaIdx, displayString.length() - commaIdx);
        std::u16string prefixResult(displayString, 0, commaIdx);
        if (russianMorphology.getDictionary().getCombinedBinaryType(&phraseType, prefixResult) != nullptr) {
            auto dictionaryResult(inflectUsingDictionary(prefixResult, phraseType, constraintStrings, std::u16string()));
            if (dictionaryResult) {
                return *dictionaryResult + suffixResult;
            }
        }
    }
    return {};
}

::inflection::dialog::DisplayValue * RuGrammarSynthesizer_RuDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    if (displayData.getValues().empty()) {
        return nullptr;
    }

    auto firstDisplayValue(displayData.getValues().front());
    const auto& displayString(firstDisplayValue.getDisplayString());
    auto caseString(remapCase(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature)));
    auto countString(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    auto genderString(GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
    auto animacyString(GrammarSynthesizerUtil::getFeatureValue(constraints, animacyFeature));
    const auto posString(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));

    std::vector<std::u16string> constraintStrings(getConstraintsVector(caseString, countString, genderString, animacyString));

    if (constraintStrings.empty() || !inflection::util::UnicodeSetUtils::containsSome(russianMorphology.inflectableChars, displayString) || inflection::util::UnicodeSetUtils::containsSome(nonRussianCyrillicChars, displayString)) {
        return new inflection::dialog::DisplayValue(firstDisplayValue);
    }

    ::std::u16string inflectionResult;
    int64_t phraseType = 0;
    if (russianMorphology.getDictionary().getCombinedBinaryType(&phraseType, displayString) != nullptr) {
        auto dictionaryResult(inflectUsingDictionary(displayString, phraseType, constraintStrings, posString));
        if (dictionaryResult) {
            inflectionResult = *dictionaryResult;
        }
    }

    std::u16string suffixResult;
    if (inflectionResult.empty() && caseString == GrammemeConstants::CASE_PREPOSITIONAL) {
        inflectionResult = getStaticSuffix(displayString, constraintStrings, suffixResult);
    }

    if (inflectionResult.empty()) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> words(npc(tokenizer->createTokenChain(displayString.substr(0, displayString.length() - suffixResult.length()))));
        const inflection::tokenizer::Token* headWordToken = nullptr;
        for (const auto &wordIter : *words) {
            if (wordIter.isSignificant()) {
                headWordToken = &wordIter;
                if (countString.empty() || genderString.empty()) {
                    int64_t wordTokenGrammemes = 0;
                    russianMorphology.getDictionary().getCombinedBinaryType(&wordTokenGrammemes, headWordToken->getValue());
                    if (countString.empty()) {
                        countString = getNumber(wordTokenGrammemes);
                    }
                    if (genderString.empty()) {
                        genderString = getGender(wordTokenGrammemes);
                    }
                }
            }
        }

        if (headWordToken != nullptr && caseString == GrammemeConstants::CASE_ACCUSATIVE && animacyString.empty()) {
            int64_t headWordGrammemes = 0;
            russianMorphology.getDictionary().getCombinedBinaryType(&headWordGrammemes, headWordToken->getValue());
            animacyString = getAnimacy(headWordGrammemes);
        }

        std::vector<std::u16string> deducedConstraints(getConstraintsVector(caseString, countString, genderString, animacyString));

        auto phraseResult = inflectPhrase(*npc(words.get()), deducedConstraints, enableInflectionGuess);

        if (!phraseResult.empty()) {
            inflectionResult = phraseResult + suffixResult;
        }
    }

    if (inflectionResult.empty()) {
        return new ::inflection::dialog::DisplayValue(firstDisplayValue);
    }

    return new ::inflection::dialog::DisplayValue(inflectionResult, constraints);
}

} // namespace inflection::grammar::synthesis
