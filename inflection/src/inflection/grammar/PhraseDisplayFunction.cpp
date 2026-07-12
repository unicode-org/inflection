/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/PhraseDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar {

using inflection::dialog::SemanticFeature;
using inflection::dialog::SemanticFeatureModel;
using inflection::dialog::DisplayValue;
using inflection::tokenizer::TokenChain;
using inflection::tokenizer::TokenizerFactory;

PhraseDisplayFunction::PhraseDisplayFunction(const SemanticFeatureModel& model, const ::inflection::util::ULocale& locale, const ::std::vector<::std::vector<::std::u16string_view>>& grammemeData, bool enableDictionaryFallback)
    : super()
    , caseFeature(*npc(model.getFeature(synthesis::GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(synthesis::GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(synthesis::GrammemeConstants::GENDER)))
    , partOfSpeechFeature(*npc(model.getFeature(synthesis::GrammemeConstants::POS)))
    , dictionaryInflector(locale, grammemeData, {}, enableDictionaryFallback)
    , tokenizer(TokenizerFactory::createTokenizer(locale))
{
}

PhraseDisplayFunction::~PhraseDisplayFunction() = default;

::std::optional<::std::u16string> PhraseDisplayFunction::inflectWord(::std::u16string_view displayString, int64_t wordType, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
{
    const auto constraintsVec(synthesis::GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&caseFeature, &numberFeature, &genderFeature}));
    const auto disambiguationGrammemeValues(synthesis::GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
    return dictionaryInflector.inflect(displayString, wordType, constraintsVec, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> PhraseDisplayFunction::inflectCompoundWord(const TokenChain& tokenChain, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
{
    ::std::u16string inflectedString;
    for (const auto& tNext : tokenChain) {
        const auto& word = tNext.getValue();
        if (!synthesis::GrammarSynthesizerUtil::checkSignificantTokenForInflection(tNext)) {
            inflectedString += word;
            continue;
        }
        int64_t wordType = 0;
        dictionaryInflector.getDictionary().getCombinedBinaryType(&wordType, word);
        inflectedString += inflectWord(word, wordType, constraints).value_or(word);
    }
    return inflectedString;
}

DisplayValue* PhraseDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = synthesis::GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    auto displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    const auto displayValueConstraints(synthesis::GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (synthesis::GrammarSynthesizerUtil::hasAnyFeatures(constraints, {&caseFeature, &numberFeature, &genderFeature})) {
        ::std::optional<::std::u16string> inflectionResult;
        int64_t wordGrammemes = 0;
        if (dictionaryInflector.getDictionary().getCombinedBinaryType(&wordGrammemes, displayString) != nullptr) {
            inflectionResult = inflectWord(displayString, wordGrammemes, constraints);
        }
        else {
            const ::std::unique_ptr<TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), constraints);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess) {
            return nullptr;
        }
    }
    return new DisplayValue(displayString, displayValueConstraints);
}

} // namespace inflection::grammar
