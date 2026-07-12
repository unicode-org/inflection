/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/analysis/RussianExposableMorphology.hpp>
#include <inflection/analysis/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/BidirectionalStringMap.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::RuGrammarSynthesizer_RuDisplayFunction final
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::analysis::RussianExposableMorphology& russianMorphology;
    ::icu4cxx::UnicodeSet nonRussianCyrillicChars { u"[:Cyrillic:]" };
    inflection::grammar::BidirectionalStringMap suffixToExemplar;

    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& animacyFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;

    //Animacy
    int64_t dictionaryAnimate {  };
    int64_t dictionaryInanimate {  };
    int64_t dictionaryAnimacyMask {  };

    //Gender
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };

    //Grammatical number
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    int64_t dictionaryNumberMask {  };

private:
    std::u16string getAnimacy(int64_t grammemes) const;
    std::u16string getGender(int64_t grammemes) const;
    std::u16string getNumber(int64_t grammemes) const;
    static std::vector<std::u16string> getConstraintsVector(const std::u16string& caseString, const std::u16string& countString, const std::u16string& genderString, const std::u16string& animacyString);
    std::optional<::std::u16string> guessInflection(const ::std::u16string &word, const std::vector<std::u16string>& constraints, const ::std::u16string& posString) const;
    std::optional<::std::u16string> inflectUsingDictionary(const ::std::u16string &word, int64_t phraseType, const std::vector<std::u16string>& constraints, const ::std::u16string& posString) const;
    std::u16string inflectPhrase(const inflection::tokenizer::TokenChain& words, const std::vector<std::u16string>& constraintStrings, bool enableInflectionGuess) const;
    std::u16string getStaticSuffix(const std::u16string &displayString, const std::vector<std::u16string>& constraintStrings, std::u16string& suffixResult) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */

    explicit RuGrammarSynthesizer_RuDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~RuGrammarSynthesizer_RuDisplayFunction() override;
    RuGrammarSynthesizer_RuDisplayFunction(const RuGrammarSynthesizer_RuDisplayFunction&) = delete;
    RuGrammarSynthesizer_RuDisplayFunction& operator=(const RuGrammarSynthesizer_RuDisplayFunction&) = delete;
};
