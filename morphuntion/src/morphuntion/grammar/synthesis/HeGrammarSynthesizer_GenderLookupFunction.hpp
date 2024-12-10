/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::HeGrammarSynthesizer_GenderLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    HeGrammarSynthesizer_GenderLookupFunction();
};
