/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/Object.hpp>
#include <map>

class MORPHUNTION_INTERNAL_API morphuntion::dialog::DefaultDisplayFunction
    : public virtual ::morphuntion::Object
{
public:
    virtual SemanticFeatureModel_DisplayValue *getDisplayValue(const SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const = 0;
    ~DefaultDisplayFunction() override;
};
