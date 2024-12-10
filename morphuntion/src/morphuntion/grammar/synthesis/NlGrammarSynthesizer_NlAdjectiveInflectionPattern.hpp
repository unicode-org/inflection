/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    virtual ::std::u16string inflectWithDeclension(const ::std::u16string& lemma, NlGrammarSynthesizer::Declension targetDeclension) const;
    virtual ::std::u16string heuristicDeclensionInflection(const ::std::u16string& lemma) const;

private:
    static bool isCombinedVowel(char16_t c1, char16_t c2);

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model);

private:
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const NlGrammarSynthesizer_NlAdjectiveInflectionPattern&) = delete;
};
