/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <memory>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryDiminutive {  };
    int64_t dictionaryNoun {  };
    int64_t dictionaryAdjective {  };
    ::std::set<::std::u16string> fugenElements { u"s" };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* sizeFeature {  };
    ::std::u16string defaultString {  };
    ::std::u16string singularNeuterString {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    bool isDiminutive(const ::std::u16string& word, int64_t phraseType) const;
    ::std::u16string determineSignificantNoun(const ::std::u16string& phrase) const;

public:
    NlGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& defaultString, const ::std::u16string& singularNeuterString);
    ~NlGrammarSynthesizer_ArticleLookupFunction() override;

private:
    NlGrammarSynthesizer_ArticleLookupFunction(const NlGrammarSynthesizer_ArticleLookupFunction&) = delete;
    NlGrammarSynthesizer_ArticleLookupFunction& operator=(const NlGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
