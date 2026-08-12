/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>

class inflection::grammar::synthesis::ArGrammarSynthesizer_PronounLookupFunction
    : public ArGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ArGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::Number countValue, ArGrammarSynthesizer::Gender genderValue, ArGrammarSynthesizer::Person personValue) const override;

    explicit ArGrammarSynthesizer_PronounLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ArGrammarSynthesizer_PronounLookupFunction(const ArGrammarSynthesizer_PronounLookupFunction&) = delete;
    ArGrammarSynthesizer_PronounLookupFunction& operator=(const ArGrammarSynthesizer_PronounLookupFunction&) = delete;
};
