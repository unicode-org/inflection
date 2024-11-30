/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::SvGrammarSynthesizer_SvDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
    , public virtual GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature& definitenessFeature;
    const ::morphuntion::dialog::SemanticFeature& countFeature;
    const ::morphuntion::dialog::SemanticFeature& caseFeature;
    const ::morphuntion::dialog::SemanticFeature& genderFeature;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    ::morphuntion::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };

private:
    ::std::u16string inflectString(const ::std::u16string& lemma, const ::std::vector<::std::u16string>& inflectionType, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string inflectTokenChain(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain) const;

public:
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    ::std::u16string inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const;
    ::std::u16string inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool targetIsNoun) const;


public: /* package */
    explicit SvGrammarSynthesizer_SvDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~SvGrammarSynthesizer_SvDisplayFunction() override;

private:
    SvGrammarSynthesizer_SvDisplayFunction(const SvGrammarSynthesizer_SvDisplayFunction&) = delete;
};
