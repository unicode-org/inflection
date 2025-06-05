/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::MlGrammarSynthesizer_CountLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {};

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    MlGrammarSynthesizer_CountLookupFunction();
    ~MlGrammarSynthesizer_CountLookupFunction() override;
    MlGrammarSynthesizer_CountLookupFunction(const MlGrammarSynthesizer_CountLookupFunction&) = delete;
    MlGrammarSynthesizer_CountLookupFunction& operator=(const MlGrammarSynthesizer_CountLookupFunction&) = delete;
};

