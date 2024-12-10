/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>

namespace morphuntion::dialog {


SemanticFeatureModel_DisplayValue::SemanticFeatureModel_DisplayValue(const ::std::u16string& displayString, const ::std::map<SemanticFeature, ::std::u16string>& constraintMap)
    : super()
    , displayString(displayString)
    , constraintMap(constraintMap)
{
}

SemanticFeatureModel_DisplayValue::SemanticFeatureModel_DisplayValue(const ::std::u16string& value)
    : SemanticFeatureModel_DisplayValue(value, {})
{
}

SemanticFeatureModel_DisplayValue::SemanticFeatureModel_DisplayValue(const SpeakableString& value, const SemanticFeature& speakFeature)
    : SemanticFeatureModel_DisplayValue(value.getPrint(), {})
{
    if (!value.speakEqualsPrint()) {
        constraintMap.emplace(speakFeature, value.getSpeak());
    }
}

SemanticFeatureModel_DisplayValue::~SemanticFeatureModel_DisplayValue()
{
}

const ::std::u16string& SemanticFeatureModel_DisplayValue::getDisplayString() const
{
    return displayString;
}

const ::std::u16string* SemanticFeatureModel_DisplayValue::getFeatureValue(const SemanticFeature& feature) const
{
    auto result = constraintMap.find(feature);
    if (result == constraintMap.end()) {
        return nullptr;
    }
    return &result->second;
}

const ::std::map<SemanticFeature, ::std::u16string>& SemanticFeatureModel_DisplayValue::getConstraintMap() const
{
    return constraintMap;
}

bool SemanticFeatureModel_DisplayValue::operator==(const SemanticFeatureModel_DisplayValue& o) const
{
    return displayString == o.displayString && constraintMap == o.constraintMap;
}

bool SemanticFeatureModel_DisplayValue::operator!=(const SemanticFeatureModel_DisplayValue& o) const
{
    return !operator==(o);
}

std::size_t SemanticFeatureModel_DisplayValue::operator()(const SemanticFeatureModel_DisplayValue& displayValue) const noexcept
{
    return std::hash<std::u16string>()(displayValue.displayString);
}

} // namespace morphuntion::dialog
