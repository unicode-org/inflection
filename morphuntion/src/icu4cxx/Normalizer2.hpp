/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
// This is a generated file. Do not edit this file.
#pragma once
#include <string_view>
#include <morphuntion/exception/ICUException.hpp>
#include <morphuntion/exception/NullPointerException.hpp>
#include <unicode/unorm2.h>
#include <icu4cxx/UnicodeSet.hpp>

namespace icu4cxx {
class Normalizer2 {
public:
static const Normalizer2  getNFCInstance() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getNFCInstance(&ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
static const Normalizer2  getNFDInstance() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getNFDInstance(&ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
static const Normalizer2  getNFKCInstance() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getNFKCInstance(&ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
static const Normalizer2  getNFKDInstance() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getNFKDInstance(&ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
static const Normalizer2  getNFKCCasefoldInstance() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getNFKCCasefoldInstance(&ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
static const Normalizer2  getInstance(const char * packageName, const char * name, UNormalization2Mode mode) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getInstance(packageName, name, mode, &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return Normalizer2(retVal, false);
}
Normalizer2(const Normalizer2 & norm2, const UnicodeSet & filterSet) {
    UErrorCode ec = U_ZERO_ERROR;
    wrappee_ = unorm2_openFiltered(norm2.wrappee_, filterSet.wrappee_, &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
}
~Normalizer2() noexcept {
    if (owned_) unorm2_close(wrappee_);
}
Normalizer2(Normalizer2 &&other) noexcept : wrappee_(other.wrappee_), owned_(other.owned_) {
    other.wrappee_ = nullptr;
}
Normalizer2& operator=(Normalizer2 &&other) noexcept {
    if (owned_) unorm2_close(wrappee_);
    wrappee_ = other.wrappee_;
    owned_ = other.owned_;
    other.wrappee_ = nullptr;
    other.owned_ = false;
    return *this;
}
int32_t normalize(std::u16string_view src, UChar * dest, int32_t capacity) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_normalize(wrappee_, (const UChar *)src.data(), (int32_t)src.size(), dest, capacity, &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t normalizeSecondAndAppend(UChar * first, int32_t firstLength, int32_t firstCapacity, std::u16string_view second) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_normalizeSecondAndAppend(wrappee_, first, firstLength, firstCapacity, (const UChar *)second.data(), (int32_t)second.size(), &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t append(UChar * first, int32_t firstLength, int32_t firstCapacity, std::u16string_view second) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_append(wrappee_, first, firstLength, firstCapacity, (const UChar *)second.data(), (int32_t)second.size(), &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t getDecomposition(UChar32 c, UChar * decomposition, int32_t capacity) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getDecomposition(wrappee_, c, decomposition, capacity, &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t getRawDecomposition(UChar32 c, UChar * decomposition, int32_t capacity) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_getRawDecomposition(wrappee_, c, decomposition, capacity, &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UChar32 composePair(UChar32 a, UChar32 b) const {
    return unorm2_composePair(wrappee_, a, b);
}
uint8_t getCombiningClass(UChar32 c) const {
    return unorm2_getCombiningClass(wrappee_, c);
}
bool isNormalized(std::u16string_view s) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_isNormalized(wrappee_, (const UChar *)s.data(), (int32_t)s.size(), &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UNormalizationCheckResult quickCheck(std::u16string_view s) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_quickCheck(wrappee_, (const UChar *)s.data(), (int32_t)s.size(), &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t spanQuickCheckYes(std::u16string_view s) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = unorm2_spanQuickCheckYes(wrappee_, (const UChar *)s.data(), (int32_t)s.size(), &ec);
    ::morphuntion::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool hasBoundaryBefore(UChar32 c) const {
    return unorm2_hasBoundaryBefore(wrappee_, c);
}
bool hasBoundaryAfter(UChar32 c) const {
    return unorm2_hasBoundaryAfter(wrappee_, c);
}
bool isInert(UChar32 c) const {
    return unorm2_isInert(wrappee_, c);
}

Normalizer2(const Normalizer2 &other) {
    if (other.owned_) {
        throw ::morphuntion::exception::NullPointerException();
    }
    wrappee_ = other.wrappee_;
    owned_ = false;
}
Normalizer2& operator= (const Normalizer2 &other) noexcept {
    Normalizer2 tmp(other);
    *this = std::move(tmp);
    return *this;
}
UNormalizer2 *wrappee_ {  };
Normalizer2(const UNormalizer2 *other, bool owned = true) : wrappee_((UNormalizer2 *)other), owned_(owned) {}
private:
bool owned_ = true;
};
} //namespace icu4xx 