/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil_SignificantTokenInflector.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <memory>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlAdjectiveListInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
    , public virtual GrammarSynthesizerUtil_SignificantTokenInflector
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    NlGrammarSynthesizer_NlAdjectiveInflectionPattern nlAdjectiveInflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlAdjectiveListInflectionPattern() override;

private:
    NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(NlGrammarSynthesizer_NlAdjectiveListInflectionPattern&) = delete;
    NlGrammarSynthesizer_NlAdjectiveListInflectionPattern& operator=(const NlGrammarSynthesizer_NlAdjectiveListInflectionPattern&) = delete;
};
