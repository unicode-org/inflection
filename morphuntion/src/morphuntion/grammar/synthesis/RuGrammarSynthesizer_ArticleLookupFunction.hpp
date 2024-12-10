/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/analysis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <map>
#include <string>

class morphuntion::grammar::synthesis::RuGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };

protected: /* package */
    virtual ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const = 0;

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    RuGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
};
