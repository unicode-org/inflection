/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <string>

class MORPHUNTION_INTERNAL_API morphuntion::dialog::DefaultFeatureFunction
    : public virtual ::morphuntion::Object
{
public:
    virtual SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const = 0;
    ~DefaultFeatureFunction() override;
};
