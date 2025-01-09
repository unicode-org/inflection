/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::HiGrammarSynthesizer_CountLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    HiGrammarSynthesizer_CountLookupFunction();
    ~HiGrammarSynthesizer_CountLookupFunction() override;

private:
    HiGrammarSynthesizer_CountLookupFunction(HiGrammarSynthesizer_CountLookupFunction&) = delete;
    HiGrammarSynthesizer_CountLookupFunction& operator=(const HiGrammarSynthesizer_CountLookupFunction &) = delete;
};
