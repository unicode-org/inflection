/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <icu4cxx/UnicodeSet.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_PossessivePattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    ::icu4cxx::UnicodeSet APOSTROPHES { u"[‘’‛'′´❛❜]" };

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    bool isApostrophe(char16_t c) const;
    bool isPossessive(const ::std::u16string& text) const;
    bool endsWithDutchSSound(const ::std::u16string& text) const;

public: /* package */
    explicit NlGrammarSynthesizer_PossessivePattern(const ::morphuntion::dialog::SemanticFeatureModel& model);

private:
    static const ::std::map<::std::u16string_view, ::std::u16string_view>& POSSESSIVE_PRONOUNS();
    static const ::std::set<::std::u16string_view>& COUNT_CONDITIONAL_PRONOUNS();
    static const ::std::set<::std::u16string_view>& DECLENSION_CONDITIONAL();
};
