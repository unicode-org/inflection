/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/grammar/BidirectionalStringMap.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <string>

class inflection::grammar::synthesis::FiGrammarSynthesizer_FiDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const inflection::grammar::BidirectionalStringMap suffixToExemplar;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& personFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    int64_t dictionaryAdjective {  };
    int64_t ignoreableGrammemes {  };
    int64_t dictionaryInflectablePOSMask {  };

public:
    std::optional<std::u16string> getExemplar(std::u16string_view word) const;
    static std::u16string_view getFallBackLemma(const ::std::u16string& unknownLemma);
private:
    ::std::u16string inflectString(const ::std::u16string& lemma, const std::u16string& posString, FiGrammarSynthesizer::Case caseValue, FiGrammarSynthesizer::Number countValue, FiGrammarSynthesizer::Person pronounPerson, bool enableInflectionGuess) const;

public:
    ::inflection::dialog::DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string getPossessiveSuffix(FiGrammarSynthesizer::Person pronounPersonValue, bool backVowels);

public: /* package */
    explicit FiGrammarSynthesizer_FiDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~FiGrammarSynthesizer_FiDisplayFunction() override;
    FiGrammarSynthesizer_FiDisplayFunction(const FiGrammarSynthesizer_FiDisplayFunction&) = delete;

private:
    friend class FiGrammarSynthesizer_LocativeBasedLookupFunction;
};
