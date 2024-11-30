/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    explicit PtGrammarSynthesizer_GenericLookupFunction(const ::std::vector<::std::u16string>& properties);
    ~PtGrammarSynthesizer_GenericLookupFunction() override;

private:
    PtGrammarSynthesizer_GenericLookupFunction(PtGrammarSynthesizer_GenericLookupFunction&) = delete;
    PtGrammarSynthesizer_GenericLookupFunction& operator=(const PtGrammarSynthesizer_GenericLookupFunction&) = delete;
};
