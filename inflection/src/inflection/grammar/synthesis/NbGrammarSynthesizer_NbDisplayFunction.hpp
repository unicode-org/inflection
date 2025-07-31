/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/grammar/synthesis/SignificantTokenInflector.hpp>
#include <string>
#include <vector>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::NbGrammarSynthesizer_NbDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
    , public virtual ::inflection::grammar::synthesis::SignificantTokenInflector
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& definitenessFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    ::inflection::dialog::DictionaryLookupFunction genderLookupFunction;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

private:
    ::std::u16string inflectNoun(const ::std::u16string& word, int64_t existingWordGrammemes, const ::std::u16string& count, const ::std::u16string& definiteness, const ::std::u16string& targetGender) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string inflectTokenChain(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain) const;

public:
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    ::std::u16string inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString, int64_t displayStringGrammemes) const;
    ::std::u16string inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, int64_t attributeDisplayStringGrammemes, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun) const;
    static ::std::u16string inflectGenitive(const ::std::u16string& string);

public: /* package */
    static ::std::u16string inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount);

    explicit NbGrammarSynthesizer_NbDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NbGrammarSynthesizer_NbDisplayFunction() override;
    NbGrammarSynthesizer_NbDisplayFunction(const NbGrammarSynthesizer_NbDisplayFunction&) = delete;
    NbGrammarSynthesizer_NbDisplayFunction& operator=(const NbGrammarSynthesizer_NbDisplayFunction&) = delete;
};
