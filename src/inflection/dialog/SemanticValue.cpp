/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticValue.hpp>

namespace inflection::dialog {


SemanticValue::SemanticValue(const ::std::u16string& label, const ::std::u16string& value)
    : label(label), value(value)
{
}

SemanticValue::~SemanticValue()
{
}

::std::u16string SemanticValue::toString() const
{
    return getLabel() + u'#' + getValue();
}

bool SemanticValue::operator==(const SemanticValue& other) const
{
    return label == other.label && value == other.value;
}

std::strong_ordering SemanticValue::operator<=>(const SemanticValue& other) const
{
    if (auto cmp = label <=> other.label; cmp != 0) {
        return cmp;
    }
    return value <=> other.value;
}

} // namespace inflection::dialog
