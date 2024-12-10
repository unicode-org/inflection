/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_CountLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_GenderLookupFunction.hpp>

class morphuntion::grammar::synthesis::ItGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::morphuntion::dialog::SemanticFeature& countFeature;
    const ::morphuntion::dialog::SemanticFeature& genderFeature;
    ItGrammarSynthesizer_CountLookupFunction countLookupFunction {  };
    ItGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    virtual ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Count countValue, ItGrammarSynthesizer::Gender genderValue) const = 0;

    ItGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
};
