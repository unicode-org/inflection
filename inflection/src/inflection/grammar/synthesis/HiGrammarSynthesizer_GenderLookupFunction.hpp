/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::HiGrammarSynthesizer_GenderLookupFunction
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

    explicit HiGrammarSynthesizer_GenderLookupFunction();
    ~HiGrammarSynthesizer_GenderLookupFunction() override;

private:
    HiGrammarSynthesizer_GenderLookupFunction(HiGrammarSynthesizer_GenderLookupFunction&) = delete;
    HiGrammarSynthesizer_GenderLookupFunction& operator=(const HiGrammarSynthesizer_GenderLookupFunction&) = delete;
};
