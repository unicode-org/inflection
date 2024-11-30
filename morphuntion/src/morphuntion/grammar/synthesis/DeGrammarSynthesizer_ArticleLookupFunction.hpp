/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <map>
#include <memory>

class morphuntion::grammar::synthesis::DeGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryCount {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    ::std::map<int32_t, ::std::u16string_view> articleMap {  };
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    ::morphuntion::grammar::synthesis::DeGrammarSynthesizer::LookupKey getArticleKey(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* displayValue) const;


public:
    DeGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::map<int32_t, ::std::u16string_view>& articleMap);
    DeGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const DeGrammarSynthesizer_ArticleLookupFunction& other);
    ~DeGrammarSynthesizer_ArticleLookupFunction() override;


private:
    DeGrammarSynthesizer_ArticleLookupFunction(DeGrammarSynthesizer_ArticleLookupFunction&) = delete;
    DeGrammarSynthesizer_ArticleLookupFunction& operator=(const DeGrammarSynthesizer_ArticleLookupFunction &) = delete;
};
