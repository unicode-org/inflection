/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <icu4cxx/UnicodeSet.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <set>
#include <string>

class morphuntion::grammar::synthesis::RuGrammarSynthesizer_InPrepositionLookupFunction
    : public RuGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef RuGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const override;

    RuGrammarSynthesizer_InPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);

private:
    static const ::icu4cxx::UnicodeSet& IN_FIRST_CHAR();

protected: /* package */
    static const::std::set<::std::u16string_view>& IN_WORDS();
};
