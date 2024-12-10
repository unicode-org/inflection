/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::grammar::synthesis::PtGrammarSynthesizer_PtDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const dialog::SemanticFeature *partOfSpeechFeature {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    PtGrammarSynthesizer_ArticleLookupFunction definiteArticleLookupFunction;
    PtGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
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
    const ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;

private:
    ::std::u16string guessPluralInflection(const std::u16string &word) const;
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflect2compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflect3compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflectCompoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::vector<::std::u16string>> inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
    ::std::optional<::std::u16string> inflectTokenChain(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;
public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit PtGrammarSynthesizer_PtDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~PtGrammarSynthesizer_PtDisplayFunction() override;

private:
    PtGrammarSynthesizer_PtDisplayFunction(const PtGrammarSynthesizer_PtDisplayFunction&) = delete;
};
