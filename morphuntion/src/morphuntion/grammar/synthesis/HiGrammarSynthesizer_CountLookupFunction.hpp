/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::HiGrammarSynthesizer_CountLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    HiGrammarSynthesizer_CountLookupFunction();
    ~HiGrammarSynthesizer_CountLookupFunction() override;

private:
    HiGrammarSynthesizer_CountLookupFunction(HiGrammarSynthesizer_CountLookupFunction&) = delete;
    HiGrammarSynthesizer_CountLookupFunction& operator=(const HiGrammarSynthesizer_CountLookupFunction &) = delete;
};
