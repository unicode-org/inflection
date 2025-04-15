/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/PtGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::PtGrammarSynthesizer_PtDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    PtGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    PtGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryPlural {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryAdjective {  };
    int64_t dictionaryVerb {  };
    int64_t dictionaryAdverb {  };
    int64_t dictionaryNoun {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryFeminine {  };
    int64_t dictionaryAdposition {  };
    int64_t dictionaryInflectablePOS {  };
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

private:
    ::std::u16string guessPluralInflection(const std::u16string &word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t wordType, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflect2compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflect3compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflectCompoundWord(const std::vector<::std::u16string> &words, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectTokenChain(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit PtGrammarSynthesizer_PtDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~PtGrammarSynthesizer_PtDisplayFunction() override;
    PtGrammarSynthesizer_PtDisplayFunction(const PtGrammarSynthesizer_PtDisplayFunction&) = delete;
    PtGrammarSynthesizer_PtDisplayFunction& operator=(const PtGrammarSynthesizer_PtDisplayFunction&) = delete;
};
