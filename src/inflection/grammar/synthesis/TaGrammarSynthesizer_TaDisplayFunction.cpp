/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/TaGrammarSynthesizer_TaDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

using inflection::dialog::SemanticFeature;
using inflection::dialog::SemanticFeatureModel;
using inflection::dialog::DisplayValue;
using inflection::tokenizer::TokenChain;
using inflection::util::LocaleUtils;
using inflection::tokenizer::TokenizerFactory;

TaGrammarSynthesizer_TaDisplayFunction::TaGrammarSynthesizer_TaDisplayFunction(const SemanticFeatureModel& model)
    : super()
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionaryInflector(LocaleUtils::TAMIL(), {
            {GrammemeConstants::POS_NOUN},
            {GrammemeConstants::ANIMACY_ANIMATE, GrammemeConstants::ANIMACY_INANIMATE},
            {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_ABLATIVE, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_SOCIATIVE, GrammemeConstants::CASE_BENEFACTIVE, GrammemeConstants::CASE_VOCATIVE},
            {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}
    }, {}, true)
    , tokenizer(TokenizerFactory::createTokenizer(LocaleUtils::TAMIL()))
{
}

TaGrammarSynthesizer_TaDisplayFunction::~TaGrammarSynthesizer_TaDisplayFunction() = default;

::std::optional<::std::u16string> TaGrammarSynthesizer_TaDisplayFunction::inflectWord(::std::u16string_view displayString, int64_t wordType, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&caseFeature, &numberFeature}));
    const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
    return dictionaryInflector.inflect(displayString, wordType, constraintsVec, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> TaGrammarSynthesizer_TaDisplayFunction::inflectCompoundWord(const TokenChain& tokenChain, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
{
    ::std::u16string inflectedString;
    for (const auto& tNext : tokenChain) {
        const auto& word = tNext.getValue();
        if (!GrammarSynthesizerUtil::checkSignificantTokenForInflection(tNext)) {
            inflectedString += word;
            continue;
        }
        int64_t wordType = 0;
        dictionaryInflector.getDictionary().getCombinedBinaryType(&wordType, word);
        inflectedString += inflectWord(word, wordType, constraints).value_or(word);
    }
    return inflectedString;
}

DisplayValue* TaGrammarSynthesizer_TaDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    auto displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    const auto displayValueConstraints(GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {&caseFeature, &numberFeature})) {
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

} // namespace inflection::grammar::synthesis
