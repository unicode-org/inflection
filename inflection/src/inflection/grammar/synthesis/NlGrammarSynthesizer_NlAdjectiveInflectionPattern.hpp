/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/SignificantTokenInflector.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveInflectionPattern
    : public virtual SignificantTokenInflector
{
public:
    typedef SignificantTokenInflector super;

private:
    const ::inflection::dialog::SemanticFeature& declensionFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryAdjective {  };

public:
    ::std::u16string inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    static ::std::u16string inflectWithDeclension(const ::std::u16string& lemma, NlGrammarSynthesizer::Declension targetDeclension);
    static ::std::u16string heuristicDeclensionInflection(const ::std::u16string& lemma);
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    static bool isCombinedVowel(char16_t c1, char16_t c2);
public:
    NlGrammarSynthesizer::Declension getDeclension(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const NlGrammarSynthesizer_NlAdjectiveInflectionPattern&) = delete;
};
