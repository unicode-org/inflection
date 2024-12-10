/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dialog/StaticArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPatternChain.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_NlDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{

public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    NlGrammarSynthesizer_InflectorPatternChain nlInflector;
    NlGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    ::morphuntion::dialog::StaticArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
};
