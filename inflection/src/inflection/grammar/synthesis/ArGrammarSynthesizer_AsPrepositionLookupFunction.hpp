/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>

class inflection::grammar::synthesis::ArGrammarSynthesizer_AsPrepositionLookupFunction
    : public ArGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ArGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const override;

    explicit ArGrammarSynthesizer_AsPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ArGrammarSynthesizer_AsPrepositionLookupFunction(const ArGrammarSynthesizer_AsPrepositionLookupFunction&) = delete;
    ArGrammarSynthesizer_AsPrepositionLookupFunction& operator=(const ArGrammarSynthesizer_AsPrepositionLookupFunction&) = delete;
};
