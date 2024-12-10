/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dialog/DeterminerAdpositionDetectionFunction.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::EsGrammarSynthesizer_EsDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    EsGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    EsGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    ::morphuntion::dialog::DeterminerAdpositionDetectionFunction determinerAdpositionDetectionFunction;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryPreposition {  };

private:
    ::std::u16string guessGenderedInflection(const ::std::u16string& word, ::std::u16string_view gender) const;
    ::std::u16string guessPluralInflection(const ::std::u16string& word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view displayString, int64_t wordType, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit EsGrammarSynthesizer_EsDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~EsGrammarSynthesizer_EsDisplayFunction() override;

private:
    EsGrammarSynthesizer_EsDisplayFunction(const EsGrammarSynthesizer_EsDisplayFunction&) = delete;
};
