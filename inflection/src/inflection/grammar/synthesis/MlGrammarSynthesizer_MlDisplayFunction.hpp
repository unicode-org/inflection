/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <optional>
#include <string>

class inflection::grammar::synthesis::MlGrammarSynthesizer_MlDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    using super = ::inflection::dialog::DefaultDisplayFunction;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer;
    const icu4cxx::UnicodeSet& malayalamInflectableChars;
    icu4cxx::UnicodeSet nonMalayalamChars;

private:
    std::vector<std::u16string> buildConstraintVector(const std::map<inflection::dialog::SemanticFeature, std::u16string> &constraints) const;
    std::optional<std::u16string> guessFallbackNounInflection(const std::u16string &phrase, const std::vector<std::u16string> &constraintValues) const;

public:
    ::inflection::dialog::DisplayValue* getDisplayValue(
        const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData,
        const std::map<::inflection::dialog::SemanticFeature, std::u16string>& constraints,
        bool enableInflectionGuess) const override;

    ::std::u16string inflectPhrase(
        const ::std::u16string& phrase,
        const ::std::vector<::std::u16string>& constraintValues,
        bool enableInflectionGuess) const;

public:
    explicit MlGrammarSynthesizer_MlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~MlGrammarSynthesizer_MlDisplayFunction() override;
    MlGrammarSynthesizer_MlDisplayFunction(MlGrammarSynthesizer_MlDisplayFunction&) = delete;
    MlGrammarSynthesizer_MlDisplayFunction& operator=(MlGrammarSynthesizer_MlDisplayFunction&) = delete;

private:
    friend class MlGrammarSynthesizer;
};
