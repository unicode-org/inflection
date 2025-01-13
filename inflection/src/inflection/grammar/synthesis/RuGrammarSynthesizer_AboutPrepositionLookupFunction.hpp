/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <string>

class inflection::grammar::synthesis::RuGrammarSynthesizer_AboutPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_AboutPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_AboutPrepositionLookupFunction(const RuGrammarSynthesizer_AboutPrepositionLookupFunction&) = delete;
    RuGrammarSynthesizer_AboutPrepositionLookupFunction& operator=(const RuGrammarSynthesizer_AboutPrepositionLookupFunction&) = delete;
};
