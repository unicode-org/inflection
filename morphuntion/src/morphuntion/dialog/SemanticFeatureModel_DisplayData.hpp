/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Provides a mapping between constraints and their corresponding SemanticValue
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SemanticFeatureModel_DisplayData
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

private:
    ::std::vector<::morphuntion::dialog::SemanticFeatureModel_DisplayValue> allValues {  };

public:
    /**
     * Returns all of the display values regardless of any constraints of a semantic features.
     */
    const ::std::vector<::morphuntion::dialog::SemanticFeatureModel_DisplayValue>& getValues() const;
    std::size_t operator()(const morphuntion::dialog::SemanticFeatureModel_DisplayData& displayData) const noexcept;
    bool operator==(const SemanticFeatureModel_DisplayData& o) const;
    bool operator!=(const SemanticFeatureModel_DisplayData& o) const;

    /**
     * Construct the display data with all of the known display values. Typically the first display value is shown that matches the desired semantic features.
     */
    explicit SemanticFeatureModel_DisplayData(const ::std::vector<::morphuntion::dialog::SemanticFeatureModel_DisplayValue>& displayValues);
    /**
     * The destructor
     */
    ~SemanticFeatureModel_DisplayData() override;

private:
    friend class SemanticFeatureModel;
    friend class SemanticFeatureModel_DisplayValue;
};
