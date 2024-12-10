/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryCommon {  };
    int64_t dictionaryNeuter {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    SvGrammarSynthesizer_GenderLookupFunction();
    ~SvGrammarSynthesizer_GenderLookupFunction() override;
};
