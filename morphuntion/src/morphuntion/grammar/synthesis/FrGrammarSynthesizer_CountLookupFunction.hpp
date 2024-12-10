/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::FrGrammarSynthesizer_CountLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };
    int64_t properNounProperty {  };
    int64_t appleProductProperty {  };
    int64_t pluralizeInvProperty {  };
    const ::morphuntion::dictionary::Inflector &inflector;

private:
    bool checkProperNoun(::std::u16string_view word) const;
    bool checkInvariantNouns(::std::u16string_view word) const;
    bool checkAppleProduct(::std::u16string_view word) const;
    ::std::optional<::std::u16string> determineWord(const std::u16string &word) const;

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    explicit FrGrammarSynthesizer_CountLookupFunction();
    ~FrGrammarSynthesizer_CountLookupFunction() override;

private:
    FrGrammarSynthesizer_CountLookupFunction(FrGrammarSynthesizer_CountLookupFunction&) = delete;
    FrGrammarSynthesizer_CountLookupFunction& operator=(const FrGrammarSynthesizer_CountLookupFunction&) = delete;
};
