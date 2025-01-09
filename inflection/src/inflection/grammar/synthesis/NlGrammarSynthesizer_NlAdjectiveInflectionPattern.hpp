/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    virtual ::std::u16string inflectWithDeclension(const ::std::u16string& lemma, NlGrammarSynthesizer::Declension targetDeclension) const;
    virtual ::std::u16string heuristicDeclensionInflection(const ::std::u16string& lemma) const;

private:
    static bool isCombinedVowel(char16_t c1, char16_t c2);

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);

private:
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const NlGrammarSynthesizer_NlAdjectiveInflectionPattern&) = delete;
};
