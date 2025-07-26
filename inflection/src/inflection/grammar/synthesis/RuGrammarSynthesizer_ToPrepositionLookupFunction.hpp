/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <string>
#include <set>

class inflection::grammar::synthesis::RuGrammarSynthesizer_ToPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

private:
    ::std::set<::std::u16string_view> toWords;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_ToPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_ToPrepositionLookupFunction(const RuGrammarSynthesizer_ToPrepositionLookupFunction&) = delete;
    RuGrammarSynthesizer_ToPrepositionLookupFunction& operator=(const RuGrammarSynthesizer_ToPrepositionLookupFunction&) = delete;
};
