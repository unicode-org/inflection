/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/DisplayValue.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SpeakableString.hpp>

namespace inflection::dialog {


DisplayValue::DisplayValue(const ::std::u16string& displayString, const ::std::map<SemanticFeature, ::std::u16string>& constraintMap)
    : super()
    , displayString(displayString)
    , constraintMap(constraintMap)
{
}

DisplayValue::DisplayValue(const ::std::u16string& value)
    : DisplayValue(value, {})
{
}

DisplayValue::DisplayValue(const SpeakableString& value, const SemanticFeature& speakFeature)
    : DisplayValue(value.getPrint(), {})
{
    if (!value.speakEqualsPrint()) {
        constraintMap.emplace(speakFeature, value.getSpeak());
    }
}

DisplayValue::~DisplayValue()
{
}

const ::std::u16string& DisplayValue::getDisplayString() const
{
    return displayString;
}

const ::std::u16string* DisplayValue::getFeatureValue(const SemanticFeature& feature) const
{
    auto result = constraintMap.find(feature);
    if (result == constraintMap.end()) {
        return nullptr;
    }
    return &result->second;
}

const ::std::map<SemanticFeature, ::std::u16string>& DisplayValue::getConstraintMap() const
{
    return constraintMap;
}

bool DisplayValue::operator==(const DisplayValue& o) const
{
    return displayString == o.displayString && constraintMap == o.constraintMap;
}

std::size_t DisplayValue::operator()(const DisplayValue& displayValue) const noexcept
{
    return std::hash<std::u16string>()(displayValue.displayString);
}

} // namespace inflection::dialog
