/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticValue.hpp>

namespace morphuntion::dialog {


SemanticValue::SemanticValue(const ::std::u16string& label, const ::std::u16string&     value)
    : super(label, value)
{
}

SemanticValue::~SemanticValue()
{
}

::std::u16string SemanticValue::toString() const
{
    return getLabel() + u'#' + getValue();
}

} // namespace morphuntion::dialog
