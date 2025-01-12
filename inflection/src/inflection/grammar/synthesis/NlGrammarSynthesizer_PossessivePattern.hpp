/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <icu4cxx/UnicodeSet.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_PossessivePattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    ::icu4cxx::UnicodeSet APOSTROPHES { u"[‘’‛'′´❛❜]" };

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    bool isApostrophe(char16_t c) const;
    bool isPossessive(const ::std::u16string& text) const;
    bool endsWithDutchSSound(const ::std::u16string& text) const;

public: /* package */
    explicit NlGrammarSynthesizer_PossessivePattern(const ::inflection::dialog::SemanticFeatureModel& model);
    NlGrammarSynthesizer_PossessivePattern(const NlGrammarSynthesizer_PossessivePattern&) = delete;
    NlGrammarSynthesizer_PossessivePattern& operator=(const NlGrammarSynthesizer_PossessivePattern&) = delete;

private:
    static const ::std::map<::std::u16string_view, ::std::u16string_view>& POSSESSIVE_PRONOUNS();
    static const ::std::set<::std::u16string_view>& COUNT_CONDITIONAL_PRONOUNS();
    static const ::std::set<::std::u16string_view>& DECLENSION_CONDITIONAL();
};
