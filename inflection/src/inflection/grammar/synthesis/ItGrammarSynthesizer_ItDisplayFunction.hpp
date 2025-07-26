/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::ItGrammarSynthesizer_ItDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const dialog::SemanticFeature *genderFeature {  };
    const dialog::SemanticFeature *partOfSpeechFeature {  };
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    ItGrammarSynthesizer_DefiniteArticleLookupFunction definiteArticleLookupFunction;
    ItGrammarSynthesizer_IndefiniteArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    int64_t dictionaryPlural {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryPreposition {  };
    int64_t dictionaryVerb {  };
    int64_t importantVerbMask {  };

private:
    bool isVerbCompatible(::std::u16string_view inflectedWord, int64_t wordGrammemes) const;

public:
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t wordGrammemes, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::inflection::tokenizer::TokenChain &tokenChain, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const;
    ::inflection::tokenizer::TokenChain& tokenize(::std::unique_ptr<::inflection::tokenizer::TokenChain>& tokenChain, const std::u16string& string) const;
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit ItGrammarSynthesizer_ItDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~ItGrammarSynthesizer_ItDisplayFunction() override;
    ItGrammarSynthesizer_ItDisplayFunction(const ItGrammarSynthesizer_ItDisplayFunction&) = delete;
    ItGrammarSynthesizer_ItDisplayFunction& operator=(const ItGrammarSynthesizer_ItDisplayFunction&) = delete;
};
