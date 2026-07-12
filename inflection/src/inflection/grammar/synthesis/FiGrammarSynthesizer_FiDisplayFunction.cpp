/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_FiDisplayFunction.hpp>

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

static constexpr int32_t MAX_SUFFIX_LEN = 8;
static constexpr int32_t MIN_STEM_LEN = 2;

FiGrammarSynthesizer_FiDisplayFunction::FiGrammarSynthesizer_FiDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::FINNISH())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::FINNISH()))
    , dictionaryInflector(::inflection::util::LocaleUtils::FINNISH(), {
        {GrammemeConstants::POS_NUMERAL, GrammemeConstants::POS_NOUN, GrammemeConstants::POS_PROPER_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB},
        {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_PARTITIVE, GrammemeConstants::CASE_INESSIVE, GrammemeConstants::CASE_ELATIVE, GrammemeConstants::CASE_ILLATIVE, GrammemeConstants::CASE_ADESSIVE, GrammemeConstants::CASE_ABLATIVE, GrammemeConstants::CASE_ALLATIVE, GrammemeConstants::CASE_ESSIVE, GrammemeConstants::CASE_TRANSLATIVE, GrammemeConstants::CASE_ABLATIVE},
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::TENSE_PRESENT, GrammemeConstants::TENSE_PAST},
        // TODO Add this back once the data has been populated.
        {/*GrammemeConstants::VERBTYPE_PARTICIPLE, GrammemeConstants::VERBTYPE_GERUND,*/ GrammemeConstants::VERBTYPE_INFINITIVE},
    }, {{GrammemeConstants::POS_VERB, u"passive"}}, true)
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FINNISH()))
    , suffixToExemplar(inflection::util::ResourceLocator::getRootForLocale(inflection::util::LocaleUtils::FINNISH()) + u"/exemplar/suffix_" + inflection::util::LocaleUtils::FINNISH().toString() + u".bist")
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , personFeature(*npc(model.getFeature(GrammemeConstants::PERSON)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
{
    // Some words are only "outer" without the part of speech. The word is otherwise inferred.
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&ignoreableGrammemes, {FiGrammarSynthesizer::OUTER}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE}));
    int64_t inflectablePOS = 0;
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&inflectablePOS, {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_NUMERAL, GrammemeConstants::POS_PROPER_NOUN, GrammemeConstants::POS_VERB}));
    dictionaryInflectablePOSMask = dictionaryAdjective | inflectablePOS;
}

FiGrammarSynthesizer_FiDisplayFunction::~FiGrammarSynthesizer_FiDisplayFunction()
{
}

std::optional<std::u16string> FiGrammarSynthesizer_FiDisplayFunction::getExemplar(std::u16string_view word) const
{
    auto wordLen = word.length();
    for (int32_t suffixLen = std::min(MAX_SUFFIX_LEN, static_cast<int32_t>(wordLen - MIN_STEM_LEN)); suffixLen > 0; --suffixLen) {
        std::u16string_view suffix = word.substr(wordLen - suffixLen);
        auto exemplarResult = suffixToExemplar.findTarget(suffix);
        if (exemplarResult.has_value()) {
            return exemplarResult;
        }
    }
    return std::nullopt;
}

std::u16string_view FiGrammarSynthesizer_FiDisplayFunction::getFallBackLemma(const ::std::u16string& unknownLemma)
{
    if (::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::FINNISH(), unknownLemma)) {
        if (!FiGrammarSynthesizer::hasBackVowels(unknownLemma)) {
            const auto lastChar = unknownLemma.back();
            if (lastChar == u'y') {
                return u"rehentely";
            }
            if (lastChar == u'i') {
                return u"kylkirivi";
            }
            if (lastChar == u'e') {
                return u"perhe";
            }
        }
        return u"Abreu";
    }
    if (FiGrammarSynthesizer::hasBackVowels(unknownLemma)) {
        return u"Sund";
    }
    return u"kennel";
}

::std::u16string FiGrammarSynthesizer_FiDisplayFunction::inflectString(const ::std::u16string& lemma, const std::u16string& posString, FiGrammarSynthesizer::Case caseValue, FiGrammarSynthesizer::Number countValue, FiGrammarSynthesizer::Person pronounPerson, bool enableInflectionGuess) const
{
    int64_t binaryType = 0;
    dictionary.getCombinedBinaryType(&binaryType, lemma);
    binaryType = binaryType & ~ignoreableGrammemes;
    if (binaryType != 0 && (binaryType & dictionaryInflectablePOSMask) == 0) {
        // We know the word, and we can't inflect it.
        return lemma;
    }
    if (pronounPerson != FiGrammarSynthesizer::Person::undefined) {
        auto possessiveSuffix = getPossessiveSuffix(pronounPerson, FiGrammarSynthesizer::hasBackVowels(lemma));
        if (caseValue == FiGrammarSynthesizer::Case::nominative || (caseValue == FiGrammarSynthesizer::Case::genitive && countValue == FiGrammarSynthesizer::Number::singular)) {
            auto wordInEssive = inflectString(lemma, posString, FiGrammarSynthesizer::Case::essive, FiGrammarSynthesizer::Number::singular, FiGrammarSynthesizer::Person::undefined, enableInflectionGuess);
            return wordInEssive.substr(0, wordInEssive.length() - 2) + possessiveSuffix;
        }
        if (caseValue == FiGrammarSynthesizer::Case::illative || (caseValue == FiGrammarSynthesizer::Case::genitive && countValue == FiGrammarSynthesizer::Number::plural)) {
            auto inflectedWord = inflectString(lemma, posString, caseValue, countValue, FiGrammarSynthesizer::Person::undefined, enableInflectionGuess);
            return inflectedWord.substr(0, inflectedWord.length() - 1) + possessiveSuffix;
        }
        if (caseValue == FiGrammarSynthesizer::Case::translative) {
            auto inflectedWord = inflectString(lemma, posString, caseValue, countValue, FiGrammarSynthesizer::Person::undefined, enableInflectionGuess);
            return inflectedWord.substr(0, inflectedWord.length() - 1) + u"e" + possessiveSuffix;
        }
        return inflectString(lemma, posString, caseValue, countValue, FiGrammarSynthesizer::Person::undefined, enableInflectionGuess) + possessiveSuffix;
    }
    std::vector<std::u16string> disambiguationGrammemeValues;
    if (!posString.empty()) {
        disambiguationGrammemeValues.emplace_back(posString);
    }
    auto desiredCase(FiGrammarSynthesizer::toString(caseValue));
    std::vector<std::u16string> constraintsVector;
    if (!desiredCase.empty()) {
        constraintsVector.emplace_back(desiredCase);
    }
    auto desiredNumber(FiGrammarSynthesizer::toString(countValue));
    if (!desiredNumber.empty()) {
        constraintsVector.emplace_back(desiredNumber);
    }

    std::u16string exemplar(lemma);
    if (binaryType == 0) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(lemma)));
        auto words = GrammarSynthesizerUtil::getSignificantWords(*tokenChain);
        if (!words.empty()) {
            ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
            inflector.getInflectionPatternsForWord(words.back(), inflectionPatterns);
            if (!inflectionPatterns.empty()) {
                // inflectable
                exemplar = words.back();
                dictionary.getCombinedBinaryType(&binaryType, exemplar);
                binaryType = binaryType & ~ignoreableGrammemes;
            }
        }
    }
    if (enableInflectionGuess && binaryType == 0) {
        auto exemplarResult = getExemplar(lemma);
        if (exemplarResult.has_value()) {
            exemplar = exemplarResult.value();
            dictionary.getCombinedBinaryType(&binaryType, exemplar);
        }

        if (binaryType == 0) {
            exemplar = getFallBackLemma(lemma);
            dictionary.getCombinedBinaryType(&binaryType, exemplar);
        }
    }
    if (binaryType != 0) {
        auto inflectionResult = dictionaryInflector.inflectExemplar(lemma, exemplar, binaryType, constraintsVector, disambiguationGrammemeValues);
        if (enableInflectionGuess && !inflectionResult.has_value() && caseValue != FiGrammarSynthesizer::Case::undefined && countValue != FiGrammarSynthesizer::Number::undefined) {
            inflectionResult = dictionaryInflector.inflectExemplar(lemma, exemplar, binaryType, {desiredCase}, disambiguationGrammemeValues);
        }
        if (inflectionResult.has_value()) {
            return *inflectionResult;
        }
    }
    return lemma;
}

::inflection::dialog::DisplayValue * FiGrammarSynthesizer_FiDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    auto caseValue = FiGrammarSynthesizer::getCase(::inflection::dialog::DefaultArticleLookupFunction::getFeatureValue(constraints, caseFeature));
    auto countValue = FiGrammarSynthesizer::getCount(::inflection::dialog::DefaultArticleLookupFunction::getFeatureValue(constraints, countFeature));
    ::std::u16string posString(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));
    auto personFeatureStr = ::inflection::dialog::DefaultArticleLookupFunction::getFeatureValue(constraints, personFeature);
    auto pronounPersonValue = FiGrammarSynthesizer::getPronounPerson(personFeatureStr);
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    ::std::u16string inflectedWord;
    if (!displayString.empty() && (caseValue != FiGrammarSynthesizer::Case::undefined || countValue != FiGrammarSynthesizer::Number::undefined)) {
        if (caseValue == FiGrammarSynthesizer::Case::undefined) {
            caseValue = FiGrammarSynthesizer::Case::nominative;
        }
        if (countValue == FiGrammarSynthesizer::Number::undefined) {
            countValue = FiGrammarSynthesizer::Number::singular;
        }
        if (displayString.find(u' ') != ::std::u16string::npos) {
            ::std::vector<::std::u16string_view> words;
            for (::inflection::util::DelimitedStringIterator wordItr(::inflection::util::StringViewUtils::trim(displayString), u" "); wordItr.hasNext(); ++wordItr) {
                ::std::u16string_view word(*wordItr);
                if (!word.empty()) {
                    words.emplace_back(word);
                }
            }
            if (words.size() == 2) {
                ::std::u16string dependantWord(words[0]);
                ::std::u16string headWord(words[1]);
                size_t dependantWordIdx = 0;
                while (dependantWordIdx < dependantWord.length()
                    && static_cast<bool>(u_isdigit(static_cast<UChar32>(util::StringViewUtils::codePointAt(dependantWord, dependantWordIdx)))))
                {
                    dependantWordIdx++;
                }
                if (dependantWordIdx == dependantWord.length()) {
                    // Inflect a quantity
                    inflectedWord = dependantWord + u' ' + inflectString(headWord, posString, caseValue, countValue, pronounPersonValue, enableInflectionGuess);
                }
                else {
                    int64_t headBinaryType = 0;
                    int64_t dependantBinaryType = 0;
                    if ((dictionary.getCombinedBinaryType(&headBinaryType, headWord) == nullptr || (headBinaryType & dictionaryInflectablePOSMask) != 0) && (dictionary.getCombinedBinaryType(&dependantBinaryType, dependantWord) != nullptr && (dependantBinaryType & dictionaryAdjective) != 0)) {
                        // Note: we aren't using the GrammarSynthesizerUtil.inflectSignificantWords here because the tokenization splits things that
                        // look like case suffixes, so we would more or less have to reconstruct the space separated words
                        inflectedWord = inflectString(dependantWord, posString, caseValue, countValue, FiGrammarSynthesizer::Person::undefined, enableInflectionGuess) + u" "
                            + inflectString(headWord, posString, caseValue, countValue, pronounPersonValue, enableInflectionGuess);
                    }
                }
            }
        }
        if (inflectedWord.empty()) {
            inflectedWord = inflectString(displayString, posString, caseValue, countValue, pronounPersonValue, enableInflectionGuess);
        }
    }
    if (inflectedWord.empty()) {
        return new ::inflection::dialog::DisplayValue(displayString, constraints);
    }
    return new ::inflection::dialog::DisplayValue(inflectedWord, constraints);
}

::std::u16string FiGrammarSynthesizer_FiDisplayFunction::getPossessiveSuffix(FiGrammarSynthesizer::Person pronounPersonValue, bool backVowels)
{
    if (FiGrammarSynthesizer::Person::second == pronounPersonValue) {
        return u"si";
    }
    else if (FiGrammarSynthesizer::Person::third == pronounPersonValue) {
        return (backVowels ? u"nsa" : u"nsä");
    }

    return {};
}

} // namespace inflection::grammar::synthesis
