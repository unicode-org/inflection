/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/analysis/RussianExposableMorphology.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <set>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::RuGrammarSynthesizer_RuDisplayFunction final
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    ::morphuntion::analysis::RussianExposableMorphology russianMorphology;
    ::icu4cxx::UnicodeSet nonRussianCyrillicChars;

    const ::morphuntion::dialog::SemanticFeature& caseFeature;
    const ::morphuntion::dialog::SemanticFeature& countFeature;
    const ::morphuntion::dialog::SemanticFeature& genderFeature;
    const ::morphuntion::dialog::SemanticFeature& animacyFeature;
    const ::morphuntion::dialog::SemanticFeature& posFeature;

private:
    ::std::u16string inflectWithML(const ::std::u16string& inputString, const ::morphuntion::tokenizer::TokenChain& words, ::std::u16string_view caseString, ::std::u16string_view countString, ::std::u16string_view genderString, ::std::u16string_view animacyString) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */

    explicit RuGrammarSynthesizer_RuDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~RuGrammarSynthesizer_RuDisplayFunction() override;
};
