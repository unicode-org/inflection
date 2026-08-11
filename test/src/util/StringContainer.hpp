/*
 * Copyright 2024 Unicode Incorporated and others. All rights reserved.
 * Created by George Rhoten on 2025-1-9
 */
#pragma once
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <string>
#include <unicode/utypes.h>

namespace util {
    template <class T, auto Function>
    class StringContainer final {
    public:
        static constexpr int32_t DEFAULT_SIZE = 16;
        std::u16string value;
        UErrorCode status { U_ZERO_ERROR };

        explicit StringContainer(T* obj) : value(DEFAULT_SIZE, 0)
        {
            extract(obj);
        }
        ~StringContainer() {
        }
        explicit operator bool() const {
            return U_SUCCESS(status);
        }
        std::string what() {
            return u_errorName(status);
        }
        bool extract(T* obj) {
            if (obj == nullptr) {
                throw ::inflection::exception::IllegalArgumentException(u"Object can not be null");
            }
            status = U_ZERO_ERROR;
            auto resultLen = Function(obj, value.data(), int32_t(value.length()), &status);
            if (resultLen > int32_t(value.length())) {
                status = U_ZERO_ERROR;
                value.resize(resultLen, 0); // +1 for null
                resultLen = Function(obj, value.data(), int32_t(value.length()), &status);
                if (resultLen > int32_t(value.length())) {
                    throw ::inflection::exception::IllegalStateException(u"Bad buffer resize");
                }
            }
            else {
                value.resize(resultLen, 0);
            }
            return U_SUCCESS(status);
        }
    private:
        StringContainer(const StringContainer&) = delete;
        StringContainer& operator=(const StringContainer&) = delete;
    };
}