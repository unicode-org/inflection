/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::HiGrammarSynthesizer_HiDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{

public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    int64_t pluralVerbMask = 0;
    int64_t pluralFeminineMask = 0;
    int64_t genderMask = 0;
    int64_t adpositionMask = 0;

public:
    ::inflection::dialog::DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::optional<::std::u16string> inflectWord(const ::std::u16string& word, int64_t wordProperties, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess, bool makeOblique) const;
    ::std::optional<::std::vector<::std::u16string>> inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectTokenChain(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    explicit HiGrammarSynthesizer_HiDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~HiGrammarSynthesizer_HiDisplayFunction() override;
    HiGrammarSynthesizer_HiDisplayFunction(const HiGrammarSynthesizer_HiDisplayFunction&) = delete;
    HiGrammarSynthesizer_HiDisplayFunction& operator=(const HiGrammarSynthesizer_HiDisplayFunction&) = delete;
};
