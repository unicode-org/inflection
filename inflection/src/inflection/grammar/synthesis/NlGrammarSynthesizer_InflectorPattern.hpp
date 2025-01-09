/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <string>

class inflection::grammar::synthesis::NlGrammarSynthesizer_InflectorPattern
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;
public:
    virtual bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    virtual ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    ~NlGrammarSynthesizer_InflectorPattern() override;
};
