/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>
#include <set>
#include <vector>

class morphuntion::grammar::synthesis::FrGrammarSynthesizer_FrDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{

public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    FrGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    FrGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    ::std::set<::std::u16string> WORDS_PREVENTING_INFLECTION {  };
    int64_t nounOrAdjectiveProperties {  };
    int64_t articlePronounProperNounPrepositionProperties {  };
    int64_t adverbProperty {  };
    int64_t verbProperty {  };
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string guessPluralInflection(const ::std::u16string& word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    bool canBeInflectedToPlural(::std::u16string_view word) const;

public:
    explicit FrGrammarSynthesizer_FrDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~FrGrammarSynthesizer_FrDisplayFunction() override;

public:
    FrGrammarSynthesizer_FrDisplayFunction(FrGrammarSynthesizer_FrDisplayFunction&) = delete;
    FrGrammarSynthesizer_FrDisplayFunction& operator=(const FrGrammarSynthesizer_FrDisplayFunction&) = delete;
};
