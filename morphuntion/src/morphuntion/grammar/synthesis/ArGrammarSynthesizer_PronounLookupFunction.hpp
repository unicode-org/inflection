/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <map>

class morphuntion::grammar::synthesis::ArGrammarSynthesizer_PronounLookupFunction
    : public ArGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ArGrammarSynthesizer_ArticleLookupFunction super;

private:
    ::std::map<int32_t, ::std::u16string_view> articleMap {  };
public: /* package */
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const override;

    ArGrammarSynthesizer_PronounLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& articleMap);
};
