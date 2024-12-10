/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::EnGrammarSynthesizer_CountLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t prepositionProperty {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    EnGrammarSynthesizer_CountLookupFunction();
    ~EnGrammarSynthesizer_CountLookupFunction() override;

private:
    EnGrammarSynthesizer_CountLookupFunction(EnGrammarSynthesizer_CountLookupFunction&) = delete;
    EnGrammarSynthesizer_CountLookupFunction& operator=(const EnGrammarSynthesizer_CountLookupFunction &) = delete;
};
