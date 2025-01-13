/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <map>

class inflection::grammar::synthesis::ArGrammarSynthesizer_PronounLookupFunction
    : public ArGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ArGrammarSynthesizer_ArticleLookupFunction super;

private:
    ::std::map<int32_t, ::std::u16string_view> articleMap {  };
public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const override;

    ArGrammarSynthesizer_PronounLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& articleMap);
    ArGrammarSynthesizer_PronounLookupFunction(const ArGrammarSynthesizer_PronounLookupFunction&) = delete;
    ArGrammarSynthesizer_PronounLookupFunction& operator=(const ArGrammarSynthesizer_PronounLookupFunction&) = delete;
};
