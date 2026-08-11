/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/DeterminerAdpositionDetectionFunction.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <string>

class inflection::grammar::synthesis::EsGrammarSynthesizer_EsDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    EsGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    EsGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    ::inflection::dialog::DeterminerAdpositionDetectionFunction determinerAdpositionDetectionFunction;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryPreposition {  };

private:
    ::std::u16string guessGenderedInflection(const ::std::u16string& word, ::std::u16string_view gender) const;
    ::std::u16string guessPluralInflection(const ::std::u16string& word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view displayString, int64_t wordType, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::inflection::tokenizer::TokenChain& tokenize(::std::unique_ptr<::inflection::tokenizer::TokenChain>& tokenChain, const std::u16string& string) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit EsGrammarSynthesizer_EsDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~EsGrammarSynthesizer_EsDisplayFunction() override;
    EsGrammarSynthesizer_EsDisplayFunction(const EsGrammarSynthesizer_EsDisplayFunction&) = delete;
    EsGrammarSynthesizer_EsDisplayFunction& operator=(const EsGrammarSynthesizer_EsDisplayFunction&) = delete;
};
