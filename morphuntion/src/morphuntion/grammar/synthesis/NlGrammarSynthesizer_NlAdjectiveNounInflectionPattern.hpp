/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/tokenizer/fwd.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
    , public virtual GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    int64_t dictionaryFeminine {  };
    int64_t dictionaryMasculine {  };
    int64_t dictionaryNeuter {  };
    int64_t dictionaryGenderMask {  };
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern nlAdjectiveInflector;
    NlGrammarSynthesizer_NlNounInflectionPattern nlNounInflector;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

private:
    NlGrammarSynthesizer::Declension getAdjectiveDeclension(NlGrammarSynthesizer::Count nounCount, NlGrammarSynthesizer::Gender nounGender) const;
    ::std::u16string getGender(const ::std::u16string& word) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern() override;
};
