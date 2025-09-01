/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::MlGrammarSynthesizer_NumberLookupFunction
   : public ::inflection::dialog::DictionaryLookupFunction
{
public:
   typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
   const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
   const ::inflection::dictionary::DictionaryMetaData& dictionary;
   int64_t nounProperty {};

public:
   ::std::u16string determine(const ::std::u16string& word) const override;

   MlGrammarSynthesizer_NumberLookupFunction();
   ~MlGrammarSynthesizer_NumberLookupFunction() override;
   MlGrammarSynthesizer_NumberLookupFunction(const MlGrammarSynthesizer_NumberLookupFunction&) = delete;
   MlGrammarSynthesizer_NumberLookupFunction& operator=(const MlGrammarSynthesizer_NumberLookupFunction&) = delete;
};

