/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::DaGrammarSynthesizer_DaDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
    , public virtual ::morphuntion::grammar::synthesis::GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* definitenessFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* posFeature {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dictionary::Inflector& inflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    ::morphuntion::dialog::DictionaryLookupFunction genderLookupFunction;

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
    ::std::u16string inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun, bool isHeadWordWithAttribute) const;
    ::std::u16string makeGenitiveWhenRequested(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const;

public: /* package */
    static ::std::u16string inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount);

    explicit DaGrammarSynthesizer_DaDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~DaGrammarSynthesizer_DaDisplayFunction() override;

private:
    DaGrammarSynthesizer_DaDisplayFunction(const DaGrammarSynthesizer_DaDisplayFunction&) = delete;
};
