/*
 * Copyright 2019-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::FiGrammarSynthesizer_CountLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const FiGrammarSynthesizer_FiDisplayFunction& displayFunction;

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    explicit FiGrammarSynthesizer_CountLookupFunction(const FiGrammarSynthesizer_FiDisplayFunction& displayFunction);
    FiGrammarSynthesizer_CountLookupFunction(const FiGrammarSynthesizer_CountLookupFunction&) = delete;
    FiGrammarSynthesizer_CountLookupFunction& operator=(const FiGrammarSynthesizer_CountLookupFunction&) = delete;
};
