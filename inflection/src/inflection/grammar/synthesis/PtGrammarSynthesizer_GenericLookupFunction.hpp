/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    explicit PtGrammarSynthesizer_GenericLookupFunction(const ::std::vector<::std::u16string>& properties);
    ~PtGrammarSynthesizer_GenericLookupFunction() override;
    PtGrammarSynthesizer_GenericLookupFunction(const PtGrammarSynthesizer_GenericLookupFunction&) = delete;
    PtGrammarSynthesizer_GenericLookupFunction& operator=(const PtGrammarSynthesizer_GenericLookupFunction&) = delete;
};
