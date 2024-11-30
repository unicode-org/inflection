/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>

class morphuntion::grammar::synthesis::ItGrammarSynthesizer_IndefiniteArticleLookupFunction
    : public ItGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ItGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Count countValue, ItGrammarSynthesizer::Gender genderValue) const override;

    ItGrammarSynthesizer_IndefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
};
