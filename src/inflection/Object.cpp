/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/Object.hpp>

#include <inflection/util/StringViewUtils.hpp>
#include <typeinfo>

namespace inflection {

::std::u16string
Object::toString() const {
    std::u16string result;
    return *util::StringViewUtils::convert(&result, typeid(*this).name());
}

Object::~Object()
{
}

} // namespace inflection
