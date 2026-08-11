/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::PtGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature* numberFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction;
    const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction;
    ::std::u16string_view defaultString {  };
    ::std::u16string_view defaultSingularString {  };
    ::std::u16string_view defaultPluralString {  };
    ::std::u16string_view singularMasculineString {  };
    ::std::u16string_view singularFeminineString {  };
    ::std::u16string_view pluralMasculineString {  };
    ::std::u16string_view pluralFeminineString {  };
public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model,
                                               const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction,
                                               const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction,
                                               const char16_t* derivedSemanticName,
                                               std::u16string_view defaultString,
                                               std::u16string_view defaultSingularString,
                                               std::u16string_view defaultPluralString,
                                               std::u16string_view singularMasculineString,
                                               std::u16string_view singularFeminineString,
                                               std::u16string_view pluralMasculineString,
                                               std::u16string_view pluralFeminineString);
    PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const PtGrammarSynthesizer_ArticleLookupFunction& other);

    ~PtGrammarSynthesizer_ArticleLookupFunction() override;
    PtGrammarSynthesizer_ArticleLookupFunction(const PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
    PtGrammarSynthesizer_ArticleLookupFunction& operator=(const PtGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
