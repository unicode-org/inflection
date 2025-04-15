/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <memory>

class inflection::grammar::synthesis::NlGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryDiminutive {  };
    int64_t dictionaryNoun {  };
    int64_t dictionaryAdjective {  };
    ::std::set<::std::u16string> fugenElements { u"s" };
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& sizeFeature;
    ::std::u16string defaultString {  };
    ::std::u16string singularNeuterString {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    bool isDiminutive(const ::std::u16string& word, int64_t phraseType) const;
    ::std::u16string determineSignificantNoun(const ::std::u16string& phrase) const;

public:
    NlGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& defaultString, const ::std::u16string& singularNeuterString);
    ~NlGrammarSynthesizer_ArticleLookupFunction() override;
    NlGrammarSynthesizer_ArticleLookupFunction(const NlGrammarSynthesizer_ArticleLookupFunction&) = delete;
    NlGrammarSynthesizer_ArticleLookupFunction& operator=(const NlGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
