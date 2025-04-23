/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/StaticArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlVerbInflectionPattern.hpp>
#include <inflection/grammar/synthesis/SignificantTokenInflector.hpp>
#include <vector>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
    , public virtual SignificantTokenInflector
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern adjectiveInflector;
    NlGrammarSynthesizer_NlNounInflectionPattern nounInflector;
    NlGrammarSynthesizer_NlVerbInflectionPattern verbInflector;
    NlGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    ::inflection::dialog::StaticArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;
    ::std::u16string inflectPhrase(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    static NlGrammarSynthesizer::Declension getAdjectiveDeclension(NlGrammarSynthesizer::Number nounCount, NlGrammarSynthesizer::Gender nounGender);
    NlGrammarSynthesizer::Gender getGender(int64_t wordGrammemes) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlDisplayFunction() override;
};
