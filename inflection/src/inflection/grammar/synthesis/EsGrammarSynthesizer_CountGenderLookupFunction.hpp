/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::EsGrammarSynthesizer_CountGenderLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

public:
    typedef enum {
        GENDER_CATEGORY,
        NUMBER_CATEGORY
    } GrammarCategoryType;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nounProperty {  };
    int64_t propertiesMask {  };
    GrammarCategoryType grammarCategoryType {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    EsGrammarSynthesizer_CountGenderLookupFunction(GrammarCategoryType categoryType, const ::std::vector<::std::u16string>& properties);
    ~EsGrammarSynthesizer_CountGenderLookupFunction() override;

private:
    EsGrammarSynthesizer_CountGenderLookupFunction(EsGrammarSynthesizer_CountGenderLookupFunction&) = delete;
    EsGrammarSynthesizer_CountGenderLookupFunction& operator=(const EsGrammarSynthesizer_CountGenderLookupFunction&) = delete;
};
