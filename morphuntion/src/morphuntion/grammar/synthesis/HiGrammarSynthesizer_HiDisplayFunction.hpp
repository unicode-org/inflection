/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::HiGrammarSynthesizer_HiDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{

public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    int64_t pluralVerbMask = 0;
    int64_t pluralFeminineMask = 0;
    int64_t genderMask = 0;
    int64_t adpositionMask = 0;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::optional<::std::u16string> inflectWord(const ::std::u16string& word, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess, bool makeOblique) const;
    ::std::optional<::std::vector<::std::u16string>> inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectTokenChain(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    explicit HiGrammarSynthesizer_HiDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~HiGrammarSynthesizer_HiDisplayFunction() override;

public:
    HiGrammarSynthesizer_HiDisplayFunction(HiGrammarSynthesizer_HiDisplayFunction&) = delete;
    HiGrammarSynthesizer_HiDisplayFunction& operator=(const HiGrammarSynthesizer_HiDisplayFunction&) = delete;
};
