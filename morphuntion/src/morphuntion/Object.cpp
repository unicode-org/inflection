/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/Object.hpp>

#include <morphuntion/util/StringViewUtils.hpp>
#include <typeinfo>

namespace morphuntion {

::std::u16string
Object::toString() const {
    std::u16string result;
    return *util::StringViewUtils::convert(&result, typeid(*this).name());
}

Object::~Object()
{
}

} // namespace morphuntion
