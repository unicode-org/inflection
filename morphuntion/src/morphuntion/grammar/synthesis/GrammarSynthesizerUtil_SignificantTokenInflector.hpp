/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <map>
#include <vector>

class morphuntion::grammar::synthesis::GrammarSynthesizerUtil_SignificantTokenInflector
    : public virtual ::morphuntion::Object
{
public:
    virtual ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const = 0;
    ~GrammarSynthesizerUtil_SignificantTokenInflector() override;
};
