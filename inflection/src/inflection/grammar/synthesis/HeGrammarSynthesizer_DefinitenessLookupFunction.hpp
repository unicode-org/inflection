/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::HeGrammarSynthesizer_DefinitenessLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    HeGrammarSynthesizer_DefinitenessLookupFunction();
};
