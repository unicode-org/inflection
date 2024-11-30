/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::ItGrammarSynthesizer_ItDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const dialog::SemanticFeature *genderFeature {  };
    const dialog::SemanticFeature *partOfSpeechFeature {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    ItGrammarSynthesizer_DefiniteArticleLookupFunction definiteArticleLookupFunction;
    ItGrammarSynthesizer_IndefiniteArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    int64_t dictionaryPlural {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryPreposition {  };

public:
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::morphuntion::tokenizer::TokenChain &tokenChain, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const;
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit ItGrammarSynthesizer_ItDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~ItGrammarSynthesizer_ItDisplayFunction() override;

private:
    ItGrammarSynthesizer_ItDisplayFunction(const ItGrammarSynthesizer_ItDisplayFunction&) = delete;
};
