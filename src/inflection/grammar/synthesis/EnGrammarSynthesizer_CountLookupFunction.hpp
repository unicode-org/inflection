/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::EnGrammarSynthesizer_CountLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t prepositionProperty {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    EnGrammarSynthesizer_CountLookupFunction();
    ~EnGrammarSynthesizer_CountLookupFunction() override;
    EnGrammarSynthesizer_CountLookupFunction(const EnGrammarSynthesizer_CountLookupFunction&) = delete;
    EnGrammarSynthesizer_CountLookupFunction& operator=(const EnGrammarSynthesizer_CountLookupFunction &) = delete;
};
