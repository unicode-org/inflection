/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <string>
#include <vector>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::NbGrammarSynthesizer_NbDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
    , public virtual ::morphuntion::grammar::synthesis::GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* definitenessFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* posFeature {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    ::morphuntion::dialog::DictionaryLookupFunction genderLookupFunction;

private:
    ::std::u16string inflectString(const ::std::u16string& word, int64_t existingWordGrammemes, const ::std::u16string& count, const ::std::u16string& definiteness, const ::std::u16string& targetGender, bool targetIsANoun) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string inflectTokenChain(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain) const;

public:
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    ::std::u16string inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString, int64_t displayStringGrammemes) const;
    ::std::u16string inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, int64_t attributeDisplayStringGrammemes, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun) const;
    static ::std::u16string inflectGenitive(const ::std::u16string& string);

public: /* package */
    static ::std::u16string inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount);

    explicit NbGrammarSynthesizer_NbDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~NbGrammarSynthesizer_NbDisplayFunction() override;

private:
    NbGrammarSynthesizer_NbDisplayFunction(const NbGrammarSynthesizer_NbDisplayFunction&) = delete;
};
