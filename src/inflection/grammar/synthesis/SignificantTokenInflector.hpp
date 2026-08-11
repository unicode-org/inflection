/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <map>
#include <vector>

class inflection::grammar::synthesis::SignificantTokenInflector
    : public virtual ::inflection::Object
{
public:
    virtual ::std::vector<::std::u16string> inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const = 0;
    ~SignificantTokenInflector() override;
};
