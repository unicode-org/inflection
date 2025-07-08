/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Provides a mapping between constraints and their corresponding SemanticValue
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureModel_DisplayData
    : public virtual ::inflection::Object
{
public:
    // @cond
    typedef ::inflection::Object super;
    // @endcond

private:
    ::std::vector<::inflection::dialog::DisplayValue> allValues {  };

public:
    /**
     * Returns all of the display values regardless of any constraints of a semantic features.
     */
    const ::std::vector<::inflection::dialog::DisplayValue>& getValues() const;
    std::size_t operator()(const inflection::dialog::SemanticFeatureModel_DisplayData& displayData) const noexcept;
    bool operator==(const SemanticFeatureModel_DisplayData& o) const;
    bool operator!=(const SemanticFeatureModel_DisplayData& o) const;

    /**
     * Construct the display data with all of the known display values. Typically the first display value is shown that matches the desired semantic features.
     */
    explicit SemanticFeatureModel_DisplayData(const ::std::vector<::inflection::dialog::DisplayValue>& displayValues);
    /**
     * The destructor
     */
    ~SemanticFeatureModel_DisplayData() override;

private:
    friend class SemanticFeatureModel;
    friend class DisplayValue;
};
