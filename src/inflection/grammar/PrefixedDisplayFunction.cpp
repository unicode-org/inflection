/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/PrefixedDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/npc.hpp>
#include <vector>

namespace inflection::grammar {

PrefixedDisplayFunction::PrefixedDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::inflection::util::ULocale& locale, const ::std::vector<::std::vector<::std::u16string_view>>& grammemeData, const ::std::u16string& extraFeatureName, PrefixGenerator prefixGenerator)
    : super()
    , caseFeature(*npc(model.getFeature(synthesis::GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(synthesis::GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(synthesis::GrammemeConstants::GENDER)))
    , extraFeature(*npc(model.getFeature(extraFeatureName)))
    , partOfSpeechFeature(*npc(model.getFeature(synthesis::GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(locale)))
    , dictionaryInflector(locale, grammemeData)
    , prefixesWithPOS(prefixGenerator != nullptr ? prefixGenerator(dictionary) : std::vector<std::tuple<std::u16string_view, int64_t, int64_t>>{})
{
}

PrefixedDisplayFunction::~PrefixedDisplayFunction()
{
}

::std::optional<::std::u16string> PrefixedDisplayFunction::inflectWord(::std::u16string_view word, int64_t& wordType, const ::std::vector<::std::u16string>& constraints, const ::std::vector<::std::u16string>& disambiguationGrammemeValues, bool /*enableInflectionGuess*/) const
{
    ::std::optional<::std::u16string> inflectedWord;
    std::u16string_view prefix;
    if (wordType == 0 && !prefixesWithPOS.empty()) {
        int64_t prefixGrammemes = 0;
        auto splitOffset = synthesis::GrammarSynthesizerUtil::splitPrefix(word, dictionary, prefixGrammemes, wordType, prefixesWithPOS);
        if (splitOffset >= 0) {
            prefix = word.substr(0, splitOffset);
            word.remove_prefix(splitOffset);
        }
    }
    if (wordType != 0) {
        inflectedWord = dictionaryInflector.inflect(word, wordType, constraints, disambiguationGrammemeValues);
        if (inflectedWord) {
            return inflectedWord->insert(0, prefix);
        }
    }
    return {};
}

::inflection::dialog::DisplayValue * PrefixedDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = synthesis::GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    const auto displayValueConstraints(synthesis::GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (!constraints.empty()) {
        int64_t wordGrammemes = 0;
        dictionary.getCombinedBinaryType(&wordGrammemes, displayString);
        const auto constraintsVec(synthesis::GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&numberFeature, &genderFeature, &caseFeature, &extraFeature}));
        const auto disambiguationGrammemeValues(synthesis::GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
        std::optional<std::u16string> inflectionResult = inflectWord(displayString, wordGrammemes, constraintsVec, disambiguationGrammemeValues, enableInflectionGuess);
        if (inflectionResult) {
            displayString = *inflectionResult;
        }
    }
    return new ::inflection::dialog::DisplayValue(displayString, displayValueConstraints);
}

} // namespace inflection::grammar
