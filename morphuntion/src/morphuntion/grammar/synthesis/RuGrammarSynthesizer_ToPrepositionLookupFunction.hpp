/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <string>
#include <set>

class morphuntion::grammar::synthesis::RuGrammarSynthesizer_ToPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_ToPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);


public: /* package */
    static const ::std::set<::std::u16string_view>& TO_WORDS();
};
