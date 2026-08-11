/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::BnGrammarSynthesizer_BnDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;

private:
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints) const;
    ::std::optional<::std::u16string> inflectCompoundWord(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

    explicit BnGrammarSynthesizer_BnDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~BnGrammarSynthesizer_BnDisplayFunction() override;
    BnGrammarSynthesizer_BnDisplayFunction(const BnGrammarSynthesizer_BnDisplayFunction&) = delete;
    BnGrammarSynthesizer_BnDisplayFunction& operator=(const BnGrammarSynthesizer_BnDisplayFunction&) = delete;
};
