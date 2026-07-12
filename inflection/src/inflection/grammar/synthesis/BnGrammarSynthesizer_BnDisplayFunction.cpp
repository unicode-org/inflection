/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/BnGrammarSynthesizer_BnDisplayFunction.hpp>

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

BnGrammarSynthesizer_BnDisplayFunction::BnGrammarSynthesizer_BnDisplayFunction(const SemanticFeatureModel& model)
    : super()
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionaryInflector(LocaleUtils::BANGLA(), {
            {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE},
            {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}
    }, {}, true)
    , tokenizer(TokenizerFactory::createTokenizer(LocaleUtils::BANGLA()))
{
}

BnGrammarSynthesizer_BnDisplayFunction::~BnGrammarSynthesizer_BnDisplayFunction() = default;

::std::optional<::std::u16string> BnGrammarSynthesizer_BnDisplayFunction::inflectWord(::std::u16string_view displayString, int64_t wordType, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&numberFeature}));
    const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
    return dictionaryInflector.inflect(displayString, wordType, constraintsVec, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> BnGrammarSynthesizer_BnDisplayFunction::inflectCompoundWord(const TokenChain& tokenChain, const ::std::map<SemanticFeature, ::std::u16string> &constraints) const
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

DisplayValue* BnGrammarSynthesizer_BnDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {&numberFeature})) {
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
