/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>

#include <inflection/dialog/DisplayValue.hpp>

namespace inflection::dialog {

SemanticFeatureModel_DisplayData::SemanticFeatureModel_DisplayData(const ::std::vector<DisplayValue>& displayValues)
    : super()
    , allValues(displayValues)
{
}

SemanticFeatureModel_DisplayData::~SemanticFeatureModel_DisplayData()
{
}

const ::std::vector<DisplayValue>& SemanticFeatureModel_DisplayData::getValues() const
{
    return allValues;
}

std::size_t SemanticFeatureModel_DisplayData::operator()(const SemanticFeatureModel_DisplayData& displayData) const noexcept
{
    size_t hash = 0;
    for (const auto& displayValue : displayData.allValues) {
        hash ^= displayValue(displayValue) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

bool SemanticFeatureModel_DisplayData::operator==(const SemanticFeatureModel_DisplayData& o) const
{
    return allValues == o.allValues;
}

} // namespace inflection::dialog
