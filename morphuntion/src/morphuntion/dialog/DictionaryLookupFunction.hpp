/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/analysis/DictionaryExposableMorphology.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <string>
#include <vector>

class morphuntion::dialog::DictionaryLookupFunction
    : public virtual DefaultFeatureFunction
{

public:
    typedef DefaultFeatureFunction super;

private:
    static ::std::vector<int64_t> convertDisambiguationPartsOfSpeech(const dictionary::DictionaryMetaData &dictionary, const ::std::vector<::std::u16string> &disambiguationPartsOfSpeech);
    ::std::u16string determineWithPropertyMask(int64_t properties) const;
public:
    virtual ::std::u16string determine(const ::std::u16string& word) const;
    virtual ::std::u16string determinePhrase(const ::std::u16string& word, bool firstWordDeterminesValue) const;
private:
    void getImportantTokens(const ::morphuntion::tokenizer::TokenChain *tokenChain, bool firstWordDeterminesValue, const ::morphuntion::tokenizer::Token_Word *& firstWordToken, const ::morphuntion::tokenizer::Token_Word *& firstRelevantToken) const;

public:
    SpeakableString* getFeatureValue(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const override;

protected: /* protected */
    const dictionary::DictionaryMetaData & getDictionary() const;

private:
    ::std::u16string determineWithDisambiguation(std::u16string_view word) const;
    int8_t countMaskedEnabledBits(int64_t bitField) const;
public:
    ::std::u16string getFirstWord(const ::std::u16string& word) const;

public:
    DictionaryLookupFunction(const ::morphuntion::util::ULocale& locale, const ::std::vector<::std::u16string>& tags);
    /* Would construct a DictionaryLookupFunction that performs disambiguation before performing a lookup*/
    DictionaryLookupFunction(const ::morphuntion::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech);

private:
    const ::morphuntion::analysis::DictionaryExposableMorphology dictionaryMorphology;
    ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t mask {  };
    /* Parts of speech along which we perform disambiguation */
    const ::std::vector<int64_t> disambiguationPartsOfSpeech { };
    bool enableDisambiguation = false;
};
