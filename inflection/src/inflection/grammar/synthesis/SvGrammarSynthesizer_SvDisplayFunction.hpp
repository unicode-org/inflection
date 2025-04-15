/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/grammar/synthesis/SignificantTokenInflector.hpp>
#include <string>
#include <vector>


class inflection::grammar::synthesis::SvGrammarSynthesizer_SvDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
    , public virtual SignificantTokenInflector
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& definitenessFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    ::inflection::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

private:
    ::std::u16string inflectString(const ::std::u16string& lemma, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string inflectTokenChain(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain) const;

public:
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    ::std::u16string inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const;
    ::std::u16string inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool targetIsNoun) const;


public: /* package */
    explicit SvGrammarSynthesizer_SvDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~SvGrammarSynthesizer_SvDisplayFunction() override;
    SvGrammarSynthesizer_SvDisplayFunction(const SvGrammarSynthesizer_SvDisplayFunction&) = delete;
    SvGrammarSynthesizer_SvDisplayFunction& operator=(const SvGrammarSynthesizer_SvDisplayFunction&) = delete;
};
