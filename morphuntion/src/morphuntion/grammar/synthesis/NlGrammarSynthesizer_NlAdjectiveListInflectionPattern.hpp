/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <memory>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveListInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
    , public virtual GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern nlAdjectiveInflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlAdjectiveListInflectionPattern() override;

private:
    NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(NlGrammarSynthesizer_NlAdjectiveListInflectionPattern&) = delete;
    NlGrammarSynthesizer_NlAdjectiveListInflectionPattern& operator=(const NlGrammarSynthesizer_NlAdjectiveListInflectionPattern&) = delete;
};
