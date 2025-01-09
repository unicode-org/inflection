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

public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);


public: /* package */
    static const ::icu4cxx::UnicodeSet& WITH_FIRST_CHAR();
    static const ::std::set<::std::u16string_view>& WITH_WORDS();
};
