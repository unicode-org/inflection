/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::RuGrammarSynthesizer_WithPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_WithPrepositionLookupFunction(const RuGrammarSynthesizer_WithPrepositionLookupFunction&) = delete;
    RuGrammarSynthesizer_WithPrepositionLookupFunction& operator=(const RuGrammarSynthesizer_WithPrepositionLookupFunction&) = delete;
};
