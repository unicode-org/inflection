/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>

class inflection::grammar::synthesis::DeGrammarSynthesizer_DeDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& stemFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& declensionFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    ::std::map<int32_t, ::std::u16string_view> strongSuffixes {  };
    ::std::map<int32_t, ::std::u16string_view> weakSuffixes {  };
    ::std::map<int32_t, ::std::u16string_view> mixedSuffixes {  };
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    DeGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    DeGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
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
    ::inflection::dialog::DisplayValue* inflectByDeclension(const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& declensionString) const;
    
    ::std::optional<::std::u16string> inflectWord(const ::std::u16string &displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::vector<::std::u16string> &deducedConstraints, bool enableInflectionGuess) const;

    std::optional<::std::pair<::std::u16string, ::std::u16string>> inflectDeterminerAndNoun(const ::std::u16string &determiner, int64_t determinerGrammemes, const ::std::u16string &noun, int64_t nounGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, bool enableInflectionGuess) const;

    ::std::optional<::std::u16string> inflectAdjectiveNextToNoun(const ::std::u16string &adjective, int64_t adjectiveType, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::u16string &nounAfterInflection) const;

    ::std::u16string getGender(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, int64_t binaryType) const;

    std::optional<::std::u16string> getFeatureNameFromConstraintsOrBinaryType(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, int64_t binaryType, int64_t mask, const ::inflection::dialog::SemanticFeature& semanticFeature) const;

    ::std::u16string getLookupDeclensionAdjective(const ::std::u16string &lemma, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, const ::std::u16string &targetGender) const;
    ::std::u16string inflectGenitiveProperNoun(const ::std::u16string &displayString) const;

    std::optional<::std::pair<::std::u16string, ::std::u16string>> inflect2Words(const ::std::u16string &dependentWord, const ::std::u16string &headWord, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::optional<::std::u16string> inflectTokenChain(const ::inflection::tokenizer::TokenChain &tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:

public: /* package */
    DeGrammarSynthesizer_DeDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& strongSuffixes, const ::std::map<int32_t, ::std::u16string_view>& weakSuffixes, const ::std::map<int32_t, ::std::u16string_view>& mixedSuffixes);
    ~DeGrammarSynthesizer_DeDisplayFunction() override;
    DeGrammarSynthesizer_DeDisplayFunction(const DeGrammarSynthesizer_DeDisplayFunction&) = delete;
    DeGrammarSynthesizer_DeDisplayFunction& operator=(const DeGrammarSynthesizer_DeDisplayFunction&) = delete;
};
