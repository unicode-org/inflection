/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <set>
#include <string>

class inflection::grammar::synthesis::RuGrammarSynthesizer_InPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

private:
    ::std::set<::std::u16string_view> inWords;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_InPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_InPrepositionLookupFunction(const RuGrammarSynthesizer_InPrepositionLookupFunction&) = delete;
    RuGrammarSynthesizer_InPrepositionLookupFunction& operator=(const RuGrammarSynthesizer_InPrepositionLookupFunction&) = delete;

private:
    static const ::icu4cxx::UnicodeSet& IN_FIRST_CHAR();
};
