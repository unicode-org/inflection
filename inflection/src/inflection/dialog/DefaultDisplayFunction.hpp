/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/Object.hpp>
#include <map>

class INFLECTION_INTERNAL_API inflection::dialog::DefaultDisplayFunction
    : public virtual ::inflection::Object
{
public:
    virtual DisplayValue *getDisplayValue(const SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const = 0;
    ~DefaultDisplayFunction() override;
};
