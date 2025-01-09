/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>
#include <vector>

class inflection::grammar::synthesis::NlGrammarSynthesizer_InflectorPatternChain
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private:
    ::std::vector<NlGrammarSynthesizer_InflectorPattern*> inflectors {  };
    bool returnOriginalIfUnsupported {  };

public:
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

    NlGrammarSynthesizer_InflectorPatternChain(const ::std::vector<NlGrammarSynthesizer_InflectorPattern*>& inflectors, bool returnOriginalIfUnsupported);
    ~NlGrammarSynthesizer_InflectorPatternChain() override;

private:
    NlGrammarSynthesizer_InflectorPatternChain(const NlGrammarSynthesizer_InflectorPatternChain&) = delete;
    NlGrammarSynthesizer_InflectorPatternChain& operator=(const NlGrammarSynthesizer_InflectorPatternChain&) = delete;
};
