/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <map>
#include <memory>

class inflection::grammar::synthesis::DeGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryCount {  };
    const ::inflection::dialog::SemanticFeature* caseFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    ::std::map<int32_t, ::std::u16string_view> articleMap {  };
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    ::inflection::grammar::synthesis::DeGrammarSynthesizer::LookupKey getArticleKey(const ::inflection::dialog::DisplayValue* displayValue) const;


public:
    DeGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::map<int32_t, ::std::u16string_view>& articleMap);
    DeGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const DeGrammarSynthesizer_ArticleLookupFunction& other);
    ~DeGrammarSynthesizer_ArticleLookupFunction() override;
    DeGrammarSynthesizer_ArticleLookupFunction(const DeGrammarSynthesizer_ArticleLookupFunction&) = delete;
    DeGrammarSynthesizer_ArticleLookupFunction& operator=(const DeGrammarSynthesizer_ArticleLookupFunction &) = delete;
};
