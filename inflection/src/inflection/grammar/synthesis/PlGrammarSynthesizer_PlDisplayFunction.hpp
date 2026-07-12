/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/BidirectionalStringMap.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::PlGrammarSynthesizer_PlDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& animacyFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dialog::SemanticFeature& withPrepositionFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const inflection::grammar::BidirectionalStringMap suffixToExemplar;
    const inflection::dialog::DictionaryLookupFunction& caseLookupFunction;
    const inflection::dialog::DictionaryLookupFunction& genderLookupFunction;
    const inflection::dialog::DictionaryLookupFunction& numberLookupFunction;
    const inflection::dialog::DictionaryLookupFunction& animacyLookupFunction;
    int64_t dictionaryAdposition {  };
    int64_t dictionaryPronoun {  };
    int64_t dictionaryAdjective {  };
    std::u16string_view NEGATED_PREFIX { u"nie" };
    std::u16string_view MOST_PREFIX { u"naj" };

private:
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t& wordType, const ::std::vector<::std::u16string>& constraints, const ::std::vector<::std::u16string>& disambiguationGrammemeValues, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectTokenChain(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit PlGrammarSynthesizer_PlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~PlGrammarSynthesizer_PlDisplayFunction() override;
    PlGrammarSynthesizer_PlDisplayFunction(const PlGrammarSynthesizer_PlDisplayFunction&) = delete;
    PlGrammarSynthesizer_PlDisplayFunction& operator=(const PlGrammarSynthesizer_PlDisplayFunction&) = delete;
};
