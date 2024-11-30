/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::EsGrammarSynthesizer_CountGenderLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

public:
    typedef enum {
        GENDER_CATEGORY,
        NUMBER_CATEGORY
    } GrammarCategoryType;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
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
