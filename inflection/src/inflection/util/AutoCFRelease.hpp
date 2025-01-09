/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <CoreFoundation/CFBase.h>

namespace inflection::util {
    template <class T, auto Destroy>
    class AutoCRelease final {
    public:
        T value;
        explicit AutoCRelease(T value) : value(value) {}
        ~AutoCRelease()
        {
            if (value != nullptr) {
                Destroy(value);
            }
        }
        explicit operator bool() const {
            return value != nullptr;
        }

        AutoCRelease& operator=(T newValue)
        {
            if (value != newValue) {
                if (value != nullptr) {
                    Destroy(value);
                }
                value = newValue;
            }
            return *this;
        }


    private:
        AutoCRelease(const AutoCRelease&) = delete;
        AutoCRelease& operator=(const AutoCRelease&) = delete;
    };

    template <class T>
    using AutoCFRelease = inflection::util::AutoCRelease<T, &CFRelease>;
}
