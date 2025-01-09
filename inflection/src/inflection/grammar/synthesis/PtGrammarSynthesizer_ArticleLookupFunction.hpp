/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/PtGrammarSynthesizer_GenericLookupFunction.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::PtGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction countLookupFunction;
    const ::inflection::grammar::synthesis::PtGrammarSynthesizer_GenericLookupFunction genderLookupFunction;
    ::std::u16string defaultString {  };
    ::std::u16string defaultSingularString {  };
    ::std::u16string defaultPluralString {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string pluralMasculineString {  };
    ::std::u16string pluralFeminineString {  };
public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultSingularString, const ::std::u16string& defaultPluralString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& pluralMasculineString, const ::std::u16string& pluralFeminineString);
    PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const PtGrammarSynthesizer_ArticleLookupFunction& other);
    ~PtGrammarSynthesizer_ArticleLookupFunction() override;

private:
    PtGrammarSynthesizer_ArticleLookupFunction(PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
    PtGrammarSynthesizer_ArticleLookupFunction& operator=(const PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
