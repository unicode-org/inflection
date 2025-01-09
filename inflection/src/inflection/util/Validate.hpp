/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <string>

class inflection::util::Validate final
{
public:
    static bool isTrue(bool b, ::std::u16string_view message);
    static bool isTrue(bool b);
    static bool isFalse(bool b);
    template <typename T>
    static T* notNull(T *obj);
    template <typename T>
    static T* notNull(T *obj, ::std::u16string_view argumentName);

private:
    Validate() = delete;
};

inline
bool inflection::util::Validate::isTrue(bool b, ::std::u16string_view message)
{
    if (!b) {
        throw ::inflection::exception::IllegalArgumentException(::std::u16string(message));
    }
    return b;
}

inline
bool inflection::util::Validate::isTrue(bool b)
{
    return isTrue(b, u"Condition is not true.");
}

inline
bool inflection::util::Validate::isFalse(bool b)
{
    if (b) {
        throw ::inflection::exception::IllegalArgumentException(u"Condition is false.");
    }
    return b;
}

template <typename T>
T* inflection::util::Validate::notNull(T *obj)
{
    if (obj == nullptr) {
        throw ::inflection::exception::NullPointerException(u"Input argument is null.");
    }
    return obj;
}

template <typename T>
T* inflection::util::Validate::notNull(T *obj, ::std::u16string_view argumentName)
{
    if (obj == nullptr) {
        throw ::inflection::exception::NullPointerException(u"Input argument <" + std::u16string(argumentName)
            + u"> is null");
    }
    return obj;
}
