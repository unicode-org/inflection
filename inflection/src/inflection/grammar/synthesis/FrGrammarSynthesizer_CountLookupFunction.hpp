/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::FrGrammarSynthesizer_CountLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };
    int64_t properNounProperty {  };
    int64_t pluralizeInvProperty {  };
    const ::inflection::dictionary::Inflector &inflector;

private:
    bool checkInvariantNouns(::std::u16string_view word, int64_t wordGrammemes) const;
    ::std::optional<::std::u16string> determineWord(const std::u16string &word) const;

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    explicit FrGrammarSynthesizer_CountLookupFunction();
    ~FrGrammarSynthesizer_CountLookupFunction() override;
    FrGrammarSynthesizer_CountLookupFunction(const FrGrammarSynthesizer_CountLookupFunction&) = delete;
    FrGrammarSynthesizer_CountLookupFunction& operator=(const FrGrammarSynthesizer_CountLookupFunction&) = delete;
};
