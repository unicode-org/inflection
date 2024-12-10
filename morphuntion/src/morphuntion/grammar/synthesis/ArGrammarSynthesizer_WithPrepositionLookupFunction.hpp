/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>

class morphuntion::grammar::synthesis::ArGrammarSynthesizer_WithPrepositionLookupFunction
    : public ArGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ArGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const override;

    explicit ArGrammarSynthesizer_WithPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
};
