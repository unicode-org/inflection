/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <string>
#include <set>

class inflection::grammar::synthesis::HeGrammarSynthesizer_HeDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& personFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dialog::SemanticFeature& definitenessFeature;
    int64_t genderMask {  };
    int64_t nounProperty {  };
    int64_t adjectiveProperty {  };
    int64_t dictionaryPreposition {  };

private:
    std::optional<::std::u16string> pluralizeFirstWordOfCompoundWithInflector(const ::std::u16string& firstWord) const;
    ::std::u16string pluralizeFirstWordOfCompoundWithHeuristics(const ::std::u16string& firstWord) const;
    ::std::u16string inflectCompoundToPlural(const ::std::u16string& stemmedSingular) const;

public:
    virtual ::std::u16string singleWordStemToPlural(::std::u16string_view stemmedSingular) const;

private:
    ::std::u16string inflectDisplayString(const ::std::u16string& displayString, const std::u16string &count, const std::u16string &gender, const std::u16string &person, const ::std::vector<::std::u16string>& disambiguationGrammemes) const;

public:
    ::inflection::dialog::DisplayValue* getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string applyDefiniteness(const ::std::u16string& input, ::std::u16string_view definiteness);

public: /* package */
    explicit HeGrammarSynthesizer_HeDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    HeGrammarSynthesizer_HeDisplayFunction(const HeGrammarSynthesizer_HeDisplayFunction&) = delete;
    HeGrammarSynthesizer_HeDisplayFunction& operator=(const HeGrammarSynthesizer_HeDisplayFunction&) = delete;
};
