/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryCommon {  };
    int64_t dictionaryNeuter {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    SvGrammarSynthesizer_GenderLookupFunction();
    ~SvGrammarSynthesizer_GenderLookupFunction() override;
};
