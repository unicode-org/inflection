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

class inflection::grammar::synthesis::RuGrammarSynthesizer_WithPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

private:
    ::std::set<::std::u16string_view> withWords;

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_WithPrepositionLookupFunction(const RuGrammarSynthesizer_WithPrepositionLookupFunction&) = delete;
    RuGrammarSynthesizer_WithPrepositionLookupFunction& operator=(const RuGrammarSynthesizer_WithPrepositionLookupFunction&) = delete;

private: /* package */
    static const ::icu4cxx::UnicodeSet& WITH_FIRST_CHAR();
};
