/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>
#include <set>
#include <vector>

class inflection::grammar::synthesis::FrGrammarSynthesizer_FrDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{

public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    FrGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    FrGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    ::std::set<::std::u16string> WORDS_PREVENTING_INFLECTION {  };
    int64_t nounOrAdjectiveProperties {  };
    int64_t articlePronounProperNounPrepositionProperties {  };
    int64_t adverbProperty {  };
    int64_t verbProperty {  };
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

private:
    ::std::u16string guessPluralInflection(const ::std::u16string& word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    bool canBeInflectedToPlural(::std::u16string_view word) const;
    ::inflection::tokenizer::TokenChain& tokenize(::std::unique_ptr<::inflection::tokenizer::TokenChain>& tokenChain, const std::u16string& string) const;

public:
    ::inflection::dialog::DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public:
    explicit FrGrammarSynthesizer_FrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~FrGrammarSynthesizer_FrDisplayFunction() override;
    FrGrammarSynthesizer_FrDisplayFunction(const FrGrammarSynthesizer_FrDisplayFunction&) = delete;
    FrGrammarSynthesizer_FrDisplayFunction& operator=(const FrGrammarSynthesizer_FrDisplayFunction&) = delete;
};
