/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_EmptyConstraintInflectorPattern
    : public virtual NlGrammarSynthesizer_InflectorPattern
{
public:
    typedef ::morphuntion::grammar::synthesis::NlGrammarSynthesizer_InflectorPattern super;
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    NlGrammarSynthesizer_EmptyConstraintInflectorPattern();
};
