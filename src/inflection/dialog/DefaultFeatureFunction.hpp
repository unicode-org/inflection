/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <string>

class INFLECTION_INTERNAL_API inflection::dialog::DefaultFeatureFunction
    : public virtual ::inflection::Object
{
public:
    virtual SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    ~DefaultFeatureFunction() override;
};
