/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/analysis/DictionaryExposableMorphology.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <string>
#include <vector>

class inflection::dialog::DictionaryLookupFunction
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
    void getImportantTokens(const ::inflection::tokenizer::TokenChain *tokenChain, bool firstWordDeterminesValue, const ::inflection::tokenizer::Token_Word *& firstWordToken, const ::inflection::tokenizer::Token_Word *& firstRelevantToken) const;

public:
    SpeakableString* getFeatureValue(const DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const override;

protected: /* protected */
    const dictionary::DictionaryMetaData & getDictionary() const;

private:
    ::std::u16string determineWithDisambiguation(std::u16string_view word) const;
public:
    ::std::u16string getFirstWord(const ::std::u16string& word) const;

public:
    DictionaryLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags);
    /* Would construct a DictionaryLookupFunction that performs disambiguation before performing a lookup*/
    DictionaryLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech);
    DictionaryLookupFunction(const DictionaryLookupFunction&) = delete;
    DictionaryLookupFunction& operator=(const DictionaryLookupFunction&) = delete;

private:
    const ::inflection::analysis::DictionaryExposableMorphology dictionaryMorphology;
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t mask {  };
    /* Parts of speech along which we perform disambiguation */
    const ::std::vector<int64_t> disambiguationPartsOfSpeech { };
    bool enableDisambiguation = false;
};
