/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
// This is a generated file. Do not edit this file.
#pragma once
#include <string_view>
#include <inflection/exception/ICUException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <unicode/uregex.h>

namespace icu4cxx {
class RegularExpression {
public:
RegularExpression(std::u16string_view pattern, uint32_t flags, UParseError * pe) {
    UErrorCode ec = U_ZERO_ERROR;
    wrappee_ = uregex_open((const UChar *)pattern.data(), (int32_t)pattern.size(), flags, pe, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
RegularExpression(UText * pattern, uint32_t flags, UParseError * pe) {
    UErrorCode ec = U_ZERO_ERROR;
    wrappee_ = uregex_openUText(pattern, flags, pe, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
RegularExpression(const char * pattern, uint32_t flags, UParseError * pe) {
    UErrorCode ec = U_ZERO_ERROR;
    wrappee_ = uregex_openC(pattern, flags, pe, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
~RegularExpression() noexcept {
    if (owned_) uregex_close(wrappee_);
}
RegularExpression(RegularExpression &&other) noexcept : wrappee_(other.wrappee_), owned_(other.owned_) {
    other.wrappee_ = nullptr;
}
RegularExpression& operator=(RegularExpression &&other) noexcept {
    if (owned_) uregex_close(wrappee_);
    wrappee_ = other.wrappee_;
    owned_ = other.owned_;
    other.wrappee_ = nullptr;
    other.owned_ = false;
    return *this;
}
RegularExpression(const RegularExpression &regexp) {
    UErrorCode ec = U_ZERO_ERROR;
    wrappee_ = uregex_clone(regexp.wrappee_, &ec);
    owned_ = true;
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
RegularExpression& operator=(const RegularExpression &other) noexcept {
    RegularExpression tmp(other);
    *this = std::move(tmp);
    return *this;
}
const UChar * pattern(int32_t * patLength) const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_pattern(wrappee_, patLength, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * patternUText() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_patternUText(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t flags() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_flags(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void setText(std::u16string_view text) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setText(wrappee_, (const UChar *)text.data(), (int32_t)text.size(), &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void setUText(UText * text) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setUText(wrappee_, text, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
const UChar * getText(int32_t * textLength) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_getText(wrappee_, textLength, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * getUText(UText * dest) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_getUText(wrappee_, dest, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void refreshUText(UText * text) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_refreshUText(wrappee_, text, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
bool matches(int32_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_matches(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool matches64(int64_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_matches64(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool lookingAt(int32_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_lookingAt(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool lookingAt64(int64_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_lookingAt64(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool find(int32_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_find(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool find64(int64_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_find64(wrappee_, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool findNext() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_findNext(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t groupCount() {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_groupCount(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t groupNumberFromName(std::u16string_view groupName) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_groupNumberFromName(wrappee_, (const UChar *)groupName.data(), (int32_t)groupName.size(), &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t groupNumberFromCName(std::string_view groupName) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_groupNumberFromCName(wrappee_, (const char *)groupName.data(), (int32_t)groupName.size(), &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t group(int32_t groupNum, UChar * dest, int32_t destCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_group(wrappee_, groupNum, dest, destCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * groupUText(int32_t groupNum, UText * dest, int64_t * groupLength) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_groupUText(wrappee_, groupNum, dest, groupLength, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t start(int32_t groupNum) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_start(wrappee_, groupNum, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int64_t start64(int32_t groupNum) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_start64(wrappee_, groupNum, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t end(int32_t groupNum) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_end(wrappee_, groupNum, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int64_t end64(int32_t groupNum) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_end64(wrappee_, groupNum, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void reset(int32_t index) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_reset(wrappee_, index, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void reset64(int64_t index) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_reset64(wrappee_, index, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void setRegion(int32_t regionStart, int32_t regionLimit) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setRegion(wrappee_, regionStart, regionLimit, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void setRegion64(int64_t regionStart, int64_t regionLimit) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setRegion64(wrappee_, regionStart, regionLimit, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void setRegionAndStart(int64_t regionStart, int64_t regionLimit, int64_t startIndex) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setRegionAndStart(wrappee_, regionStart, regionLimit, startIndex, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
int32_t regionStart() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_regionStart(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int64_t regionStart64() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_regionStart64(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t regionEnd() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_regionEnd(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int64_t regionEnd64() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_regionEnd64(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool hasTransparentBounds() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_hasTransparentBounds(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void useTransparentBounds(bool b) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_useTransparentBounds(wrappee_, b, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
bool hasAnchoringBounds() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_hasAnchoringBounds(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void useAnchoringBounds(bool b) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_useAnchoringBounds(wrappee_, b, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
bool hitEnd() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_hitEnd(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
bool requireEnd() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_requireEnd(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t replaceAll(std::u16string_view replacementText, UChar * destBuf, int32_t destCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_replaceAll(wrappee_, (const UChar *)replacementText.data(), (int32_t)replacementText.size(), destBuf, destCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * replaceAllUText(UText * replacement, UText * dest) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_replaceAllUText(wrappee_, replacement, dest, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t replaceFirst(std::u16string_view replacementText, UChar * destBuf, int32_t destCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_replaceFirst(wrappee_, (const UChar *)replacementText.data(), (int32_t)replacementText.size(), destBuf, destCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * replaceFirstUText(UText * replacement, UText * dest) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_replaceFirstUText(wrappee_, replacement, dest, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t appendReplacement(std::u16string_view replacementText, UChar * * destBuf, int32_t * destCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_appendReplacement(wrappee_, (const UChar *)replacementText.data(), (int32_t)replacementText.size(), destBuf, destCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void appendReplacementUText(UText * replacementText, UText * dest) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_appendReplacementUText(wrappee_, replacementText, dest, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
int32_t appendTail(UChar * * destBuf, int32_t * destCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_appendTail(wrappee_, destBuf, destCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
UText * appendTailUText(UText * dest) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_appendTailUText(wrappee_, dest, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t split(UChar * destBuf, int32_t destCapacity, int32_t * requiredCapacity, UChar ** destFields, int32_t destFieldsCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_split(wrappee_, destBuf, destCapacity, requiredCapacity, destFields, destFieldsCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
int32_t splitUText(UText ** destFields, int32_t destFieldsCapacity) {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_splitUText(wrappee_, destFields, destFieldsCapacity, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void setTimeLimit(int32_t limit) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setTimeLimit(wrappee_, limit, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
int32_t getTimeLimit() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_getTimeLimit(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void setStackLimit(int32_t limit) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setStackLimit(wrappee_, limit, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
int32_t getStackLimit() const {
    UErrorCode ec = U_ZERO_ERROR;
    auto retVal = uregex_getStackLimit(wrappee_, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
    return retVal;
}
void setMatchCallback(URegexMatchCallback * callback, const void * context) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setMatchCallback(wrappee_, callback, context, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void getMatchCallback(URegexMatchCallback * * callback, const void * * context) const {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_getMatchCallback(wrappee_, callback, context, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void setFindProgressCallback(URegexFindProgressCallback * callback, const void * context) {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_setFindProgressCallback(wrappee_, callback, context, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}
void getFindProgressCallback(URegexFindProgressCallback * * callback, const void * * context) const {
    UErrorCode ec = U_ZERO_ERROR;
    uregex_getFindProgressCallback(wrappee_, callback, context, &ec);
    ::inflection::exception::ICUException::throwOnFailure(ec);
}

URegularExpression *wrappee_ {  };
RegularExpression(const URegularExpression *other, bool owned = true) : wrappee_((URegularExpression *)other), owned_(owned) {}
private:
bool owned_ = true;
};
} //namespace icu4xx 