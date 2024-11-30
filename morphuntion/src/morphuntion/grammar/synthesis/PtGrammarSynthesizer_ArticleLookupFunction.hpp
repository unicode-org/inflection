/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer_GenericLookupFunction.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::PtGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction countLookupFunction;
    const ::morphuntion::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction genderLookupFunction;
    ::std::u16string defaultString {  };
    ::std::u16string defaultSingularString {  };
    ::std::u16string defaultPluralString {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string pluralMasculineString {  };
    ::std::u16string pluralFeminineString {  };
public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    PtGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultSingularString, const ::std::u16string& defaultPluralString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& pluralMasculineString, const ::std::u16string& pluralFeminineString);
    PtGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const PtGrammarSynthesizer_ArticleLookupFunction& other);
    ~PtGrammarSynthesizer_ArticleLookupFunction() override;

private:
    PtGrammarSynthesizer_ArticleLookupFunction(PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
    PtGrammarSynthesizer_ArticleLookupFunction& operator=(const PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
