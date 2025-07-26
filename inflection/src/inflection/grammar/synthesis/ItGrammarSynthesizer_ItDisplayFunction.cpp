/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_ItDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>
#include <memory>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

namespace inflection::grammar::synthesis {

ItGrammarSynthesizer_ItDisplayFunction::ItGrammarSynthesizer_ItDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , partOfSpeechFeature(model.getFeature(GrammemeConstants::POS))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::ITALIAN())))
    , dictionaryInflector(::inflection::util::LocaleUtils::ITALIAN(), {
                {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
                {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
                {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()},
                {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
        }, {}, true)
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ITALIAN()))
    , definiteArticleLookupFunction(model, ItGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const ItGrammarSynthesizer_DefiniteArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(ItGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, ItGrammarSynthesizer::ARTICLE_INDEFINITE)
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, ItGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, ItGrammarSynthesizer::ARTICLE_INDEFINITE)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {GrammemeConstants::GENDER_MASCULINE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {GrammemeConstants::GENDER_FEMININE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPreposition, {GrammemeConstants::POS_ADPOSITION()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryVerb, {GrammemeConstants::POS_VERB()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&importantVerbMask, {
        GrammemeConstants::TENSE_PAST(), GrammemeConstants::VERBTYPE_PARTICIPLE()
    }));
}

ItGrammarSynthesizer_ItDisplayFunction::~ItGrammarSynthesizer_ItDisplayFunction()
{
}

/* This better not be switching from or to a past participle.
 */
bool ItGrammarSynthesizer_ItDisplayFunction::isVerbCompatible(::std::u16string_view inflectedWord, int64_t wordGrammemes) const
{
    int64_t inflectedWordGrammemes = 0;
    dictionary.getCombinedBinaryType(&inflectedWordGrammemes, inflectedWord);
    return (inflectedWordGrammemes & dictionaryVerb) == 0 ||
        ((wordGrammemes & importantVerbMask) == (inflectedWordGrammemes & importantVerbMask));
}

::std::optional<::std::u16string> ItGrammarSynthesizer_ItDisplayFunction::inflectWord(::std::u16string_view word, int64_t wordGrammemes, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const {
    ::std::vector<::std::u16string> constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature, genderFeature}));
    ::std::vector<::std::u16string> disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));
    const auto inflectedWord(dictionaryInflector.inflect(word, wordGrammemes, constraintsVec, disambiguationGrammemeValues));
    if (inflectedWord) {
        if ((wordGrammemes & dictionaryVerb) == 0 || isVerbCompatible(*inflectedWord, wordGrammemes)) {
            return *inflectedWord;
        }
        /* We've tried to change the grammemes of a known verb incorrectly.
         * Return the original content instead.
         */
        enableInflectionGuess = true;
    }
    if (enableInflectionGuess) {
        return ::std::u16string(word);
    }
    return  {};
}

::std::optional<::std::u16string> ItGrammarSynthesizer_ItDisplayFunction::inflectCompoundWord(const inflection::tokenizer::TokenChain &tokenChain, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const {
    auto containsPrep = false;
    ::std::u16string inflectedString;
    inflectedString.reserve(npc(tokenChain.getEnd())->getEndChar());
    for (const auto& tNext : tokenChain) {
        const auto& word = tNext.getValue();
        if (!tNext.isSignificant() || word == u"-" || containsPrep || (inflectedString.empty() && word.length() == 1 && u_tolower(word[0]) == u'l')) {
            inflectedString += word;
            continue;
        }
        int64_t wordType = 0;
        dictionary.getCombinedBinaryType(&wordType, word);
        if ((wordType & dictionaryPreposition) != 0) {
            containsPrep = true;
            inflectedString += word;
            continue;
        }
        const auto inflectionResult = inflectWord(word, wordType, constraints, enableInflectionGuess);
        if (inflectionResult) {
            inflectedString += *inflectionResult;
        } else if (!enableInflectionGuess) {
            return {};
        } else {
            inflectedString += word;
        }
    }
    return inflectedString;
}

::inflection::tokenizer::TokenChain&
ItGrammarSynthesizer_ItDisplayFunction::tokenize(::std::unique_ptr<::inflection::tokenizer::TokenChain>& tokenChain, const std::u16string& string) const
{
    tokenChain.reset(npc(npc(tokenizer.get())->createTokenChain(string)));
    return *tokenChain;
}

::inflection::dialog::DisplayValue * ItGrammarSynthesizer_ItDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    auto displayValueConstraints(GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {countFeature, genderFeature})) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain;
        ::std::optional<::std::u16string> inflectionResult;
        int64_t wordType = 0;
        if (dictionary.getCombinedBinaryType(&wordType, displayString) != nullptr
            || tokenize(tokenChain, displayString).getWordCount() == 1)
        {
            inflectionResult = inflectWord(displayString, wordType, constraints, enableInflectionGuess);
        } else {
            inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), displayValueConstraints, enableInflectionGuess);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess) {
            return nullptr;
        }
    }
    return definitenessDisplayFunction.updateDefiniteness(new ::inflection::dialog::DisplayValue(displayString, displayValueConstraints), displayValueConstraints);
}

} // namespace inflection::grammar::synthesis
