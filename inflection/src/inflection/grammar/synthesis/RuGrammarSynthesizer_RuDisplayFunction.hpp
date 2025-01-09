/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/analysis/RussianExposableMorphology.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <set>
#include <string>
#include <vector>

class inflection::grammar::synthesis::RuGrammarSynthesizer_RuDisplayFunction final
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    ::inflection::analysis::RussianExposableMorphology russianMorphology;
    ::icu4cxx::UnicodeSet nonRussianCyrillicChars;

    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& animacyFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;

private:
    ::std::u16string inflectWithML(const ::std::u16string& inputString, const ::inflection::tokenizer::TokenChain& words, ::std::u16string_view caseString, ::std::u16string_view countString, ::std::u16string_view genderString, ::std::u16string_view animacyString) const;

public:
    ::inflection::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */

    explicit RuGrammarSynthesizer_RuDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~RuGrammarSynthesizer_RuDisplayFunction() override;
};
