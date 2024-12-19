/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <string>
#include <set>
#include <vector>

class morphuntion::grammar::synthesis::SrGrammarSynthesizer_SrDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{

public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public:
    explicit SrGrammarSynthesizer_SrDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~SrGrammarSynthesizer_SrDisplayFunction() override;

public:
    SrGrammarSynthesizer_SrDisplayFunction(SrGrammarSynthesizer_SrDisplayFunction&) = delete;
    SrGrammarSynthesizer_SrDisplayFunction& operator=(const SrGrammarSynthesizer_SrDisplayFunction&) = delete;
};
