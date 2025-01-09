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

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::dialog::SemanticFeature* partOfSpeechFeature {  };
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

public:
    ::inflection::dialog::SemanticFeatureModel_DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public:
    explicit SrGrammarSynthesizer_SrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~SrGrammarSynthesizer_SrDisplayFunction() override;

public:
    SrGrammarSynthesizer_SrDisplayFunction(SrGrammarSynthesizer_SrDisplayFunction&) = delete;
    SrGrammarSynthesizer_SrDisplayFunction& operator=(const SrGrammarSynthesizer_SrDisplayFunction&) = delete;
};
