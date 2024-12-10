/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>
#include <vector>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_InflectorPatternChain
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

private:
    ::std::vector<NlGrammarSynthesizer_InflectorPattern*> inflectors {  };
    bool returnOriginalIfUnsupported {  };

public:
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const;

    NlGrammarSynthesizer_InflectorPatternChain(const ::std::vector<NlGrammarSynthesizer_InflectorPattern*>& inflectors, bool returnOriginalIfUnsupported);
    ~NlGrammarSynthesizer_InflectorPatternChain() override;

private:
    NlGrammarSynthesizer_InflectorPatternChain(const NlGrammarSynthesizer_InflectorPatternChain&) = delete;
    NlGrammarSynthesizer_InflectorPatternChain& operator=(const NlGrammarSynthesizer_InflectorPatternChain&) = delete;
};
