/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/StaticArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_ArticleLookupFunction.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{

public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    ::std::vector<NlGrammarSynthesizer_InflectorPattern*> inflectors {  };
    NlGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    ::inflection::dialog::StaticArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
};
