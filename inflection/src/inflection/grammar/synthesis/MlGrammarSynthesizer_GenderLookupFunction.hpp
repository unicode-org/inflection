/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::MlGrammarSynthesizer_GenderLookupFunction
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

    explicit MlGrammarSynthesizer_GenderLookupFunction();
    ~MlGrammarSynthesizer_GenderLookupFunction() override;
    MlGrammarSynthesizer_GenderLookupFunction(const MlGrammarSynthesizer_GenderLookupFunction&) = delete;
    MlGrammarSynthesizer_GenderLookupFunction& operator=(const MlGrammarSynthesizer_GenderLookupFunction&) = delete;
};

