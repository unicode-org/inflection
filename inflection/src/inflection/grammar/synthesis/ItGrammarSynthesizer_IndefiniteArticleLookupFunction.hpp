/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>

class inflection::grammar::synthesis::ItGrammarSynthesizer_IndefiniteArticleLookupFunction
    : public ItGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ItGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const override;

    ItGrammarSynthesizer_IndefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    ItGrammarSynthesizer_IndefiniteArticleLookupFunction(const ItGrammarSynthesizer_IndefiniteArticleLookupFunction&) = delete;
    ItGrammarSynthesizer_IndefiniteArticleLookupFunction& operator=(const ItGrammarSynthesizer_IndefiniteArticleLookupFunction&) = delete;
};
