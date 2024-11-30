/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <string>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_InflectorPattern
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;
public:
    virtual bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    virtual ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    ~NlGrammarSynthesizer_InflectorPattern() override;
};
