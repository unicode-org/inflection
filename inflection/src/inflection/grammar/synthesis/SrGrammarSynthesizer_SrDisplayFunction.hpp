/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <string>
#include <set>
#include <vector>

class inflection::grammar::synthesis::SrGrammarSynthesizer_SrDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{

public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

    ::inflection::dialog::DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

    explicit SrGrammarSynthesizer_SrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~SrGrammarSynthesizer_SrDisplayFunction() override;

    SrGrammarSynthesizer_SrDisplayFunction(SrGrammarSynthesizer_SrDisplayFunction&) = delete;
    SrGrammarSynthesizer_SrDisplayFunction& operator=(const SrGrammarSynthesizer_SrDisplayFunction&) = delete;

private:
    ::std::u16string inflectFromDictionary(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& lemma) const;
    ::std::u16string inflectWithRule(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& lemma) const;

    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
};
