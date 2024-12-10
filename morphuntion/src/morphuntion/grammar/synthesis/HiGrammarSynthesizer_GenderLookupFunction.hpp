/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::HiGrammarSynthesizer_GenderLookupFunction
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

    explicit HiGrammarSynthesizer_GenderLookupFunction();
    ~HiGrammarSynthesizer_GenderLookupFunction() override;

private:
    HiGrammarSynthesizer_GenderLookupFunction(HiGrammarSynthesizer_GenderLookupFunction&) = delete;
    HiGrammarSynthesizer_GenderLookupFunction& operator=(const HiGrammarSynthesizer_GenderLookupFunction&) = delete;
};
