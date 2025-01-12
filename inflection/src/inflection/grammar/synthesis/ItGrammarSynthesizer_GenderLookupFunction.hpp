/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::ItGrammarSynthesizer_GenderLookupFunction
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

    explicit ItGrammarSynthesizer_GenderLookupFunction();
    ~ItGrammarSynthesizer_GenderLookupFunction() override;
    ItGrammarSynthesizer_GenderLookupFunction(const ItGrammarSynthesizer_GenderLookupFunction&) = delete;
    ItGrammarSynthesizer_GenderLookupFunction& operator=(const ItGrammarSynthesizer_GenderLookupFunction&) = delete;
};
