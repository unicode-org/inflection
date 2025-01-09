/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
    , public virtual GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern nlAdjectiveInflector;
    NlGrammarSynthesizer_NlNounInflectionPattern nlNounInflector;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    NlGrammarSynthesizer::Declension getAdjectiveDeclension(NlGrammarSynthesizer::Count nounCount, NlGrammarSynthesizer::Gender nounGender) const;
    ::std::u16string getGender(const ::std::u16string& word) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern() override;
};
