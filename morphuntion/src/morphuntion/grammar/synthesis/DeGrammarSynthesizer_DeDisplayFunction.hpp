/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>

class morphuntion::grammar::synthesis::DeGrammarSynthesizer_DeDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* stemFeature {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* declensionFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    ::std::map<int32_t, ::std::u16string_view> strongSuffixes {  };
    ::std::map<int32_t, ::std::u16string_view> weakSuffixes {  };
    ::std::map<int32_t, ::std::u16string_view> mixedSuffixes {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dictionary::Inflector &inflector;
    ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    DeGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    DeGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };
    int64_t dictionaryVerb {  };
    int64_t dictionaryDeterminer {  };

    int64_t dictionaryGenderMask {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };

    int64_t dictionaryCaseMask {  };
    int64_t dictionaryNominative {  };
    int64_t dictionaryGenitive {  };
    int64_t dictionaryDative {  };
    int64_t dictionaryAccusative {  };

    int64_t dictionaryCountMask {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };

private:
    const ::std::map<int32_t, ::std::u16string_view>* getSuffixMap(::std::u16string_view declensionClass) const;
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* inflectByDeclension(const ::morphuntion::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& declensionString) const;
    
    ::std::optional<::std::u16string> inflectWord(const ::std::u16string &displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::vector<::std::u16string> &deducedConstraints, bool enableInflectionGuess) const;

    std::optional<::std::pair<::std::u16string, ::std::u16string>> inflectDeterminerAndNoun(const ::std::u16string &determiner, const ::std::u16string &noun, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, bool enableInflectionGuess) const;

    ::std::optional<::std::u16string> inflectAdjectiveNextToNoun(const ::std::u16string &adjective, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::u16string &nounAfterInflection) const;

    ::std::u16string getGender(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, int64_t binaryType) const;

    std::optional<::std::u16string> getFeatureNameFromConstraintsOrBinaryType(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, int64_t binaryType, int64_t mask, const ::morphuntion::dialog::SemanticFeature* semanticFeature) const;

    ::std::u16string getLookupDeclensionAdjective(const ::std::u16string &lemma, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::u16string &targetGender) const;
    ::std::u16string inflectGenitiveProperNoun(const ::std::u16string &displayString) const;

    std::optional<::std::pair<::std::u16string, ::std::u16string>> inflect2Words(const ::std::u16string &dependentWord, const ::std::u16string &headWord, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::optional<::std::u16string> inflectTokenChain(const ::morphuntion::tokenizer::TokenChain &tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:

public: /* package */
    DeGrammarSynthesizer_DeDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& strongSuffixes, const ::std::map<int32_t, ::std::u16string_view>& weakSuffixes, const ::std::map<int32_t, ::std::u16string_view>& mixedSuffixes);
    ~DeGrammarSynthesizer_DeDisplayFunction() override;

private:
    DeGrammarSynthesizer_DeDisplayFunction(const DeGrammarSynthesizer_DeDisplayFunction&) = delete;
};
