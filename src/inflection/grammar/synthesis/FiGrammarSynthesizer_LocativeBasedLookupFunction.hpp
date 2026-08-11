/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::FiGrammarSynthesizer_LocativeBasedLookupFunction
    : public virtual ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t nouns {  };
    int64_t pluralProperty {  };
    int64_t outerProperty {  };
    int64_t adjectiveProperty {  };
    const FiGrammarSynthesizer_FiDisplayFunction& displayFunction;
    FiGrammarSynthesizer::Case caseForOuter {  };
    FiGrammarSynthesizer::Case caseForInner {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    ::std::vector<::std::vector<::std::vector<::std::u16string>>> getDictionaryCombinations(const ::std::vector<::std::vector<::std::vector<::std::u16string>>>& possibleCombinations) const;
    bool isInDictionary(const ::std::vector<::std::vector<::std::u16string>>& possibleCombination) const;
    ::std::u16string getLocativeString(const ::std::u16string& displayString) const;
    ::std::u16string getLocativeString(const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString) const;


public:
    FiGrammarSynthesizer_LocativeBasedLookupFunction(const FiGrammarSynthesizer_FiDisplayFunction& displayFunction, FiGrammarSynthesizer::Case caseForInner, FiGrammarSynthesizer::Case caseForOuter);
    FiGrammarSynthesizer_LocativeBasedLookupFunction(const FiGrammarSynthesizer_LocativeBasedLookupFunction&) = delete;
    FiGrammarSynthesizer_LocativeBasedLookupFunction& operator=(const FiGrammarSynthesizer_LocativeBasedLookupFunction&) = delete;
};
