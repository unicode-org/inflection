/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
// Ignore the deprecation warning around ucnv_safeClone
#include <unicode/umachine.h>
#undef U_DEPRECATED
#define U_DEPRECATED U_CAPI

#include <morphuntion/dictionary/metadata/CharsetConverter.hpp>

#include <morphuntion/exception/ICUException.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dictionary::metadata {

CharsetConverter::CharsetConverter(const char* encoding)
{
    auto errorCode = U_ZERO_ERROR;
    converter = ucnv_open(npc(encoding), &errorCode);
    if (U_FAILURE(errorCode)) {
        throw morphuntion::exception::ICUException(errorCode, u"Could not initialize encoding conversion for encoding " + morphuntion::util::StringViewUtils::to_u16string(encoding));
    }
    converterType = ucnv_getType(converter);
    if (converterType == UCNV_UTF8) {
        // This is unnecessary. Use other methods with fewer memory allocations.
        ucnv_close(converter);
        converter = nullptr;
    }
}

CharsetConverter::~CharsetConverter()
{
    ucnv_close(converter);
}

void CharsetConverter::encode(::std::string* out, std::u16string_view str) const
{
    if (converterType == UCNV_UTF8) {
        morphuntion::util::StringViewUtils::convert(out, str);
    }
    else if (converterType == UCNV_UTF16_LittleEndian && !U_IS_BIG_ENDIAN) {
        npc(out)->assign((const char*)str.data(), str.length() * sizeof(char16_t));
    }
    else {
        auto status = U_ZERO_ERROR;
        char converterBuffer[U_CNV_SAFECLONE_BUFFERSIZE];
        int32_t converterLength = sizeof(converterBuffer);
        auto localConverter = ucnv_safeClone(converter, converterBuffer, &converterLength, &status);
        morphuntion::exception::ICUException::throwOnFailure(status, u"Could not clone converter");

        int32_t inputLength = int32_t(str.length());
        constexpr uint8_t PADDING = 8; // + padding for possible shift state
        auto length = inputLength + PADDING;

        npc(out)->resize(length, 0);
        // attempt to convert with a best guess buffer length.
        // if necessary, increase buffer and re-run
        for (int32_t i = 0 ; i < 2; ++i) {
            auto previousLen = length;
            status = U_ZERO_ERROR;
            length = ucnv_fromUChars(localConverter, &((*out)[0]), length, (const UChar*) str.data(),
                                     inputLength, &status);
            if (previousLen != length) {
                npc(out)->resize(length, 0);
            }
            if (status != U_BUFFER_OVERFLOW_ERROR) {
                break;
            }
            // length got updated with the correct size, so redo
        }

        ucnv_close(localConverter);
        morphuntion::exception::ICUException::throwOnFailure(status, u"Conversion failed");
    }
}

void CharsetConverter::decode(::std::u16string* out, const char* str, int32_t length) const
{
    if (converterType == UCNV_UTF8) {
        morphuntion::util::StringViewUtils::convert(out, std::string_view(str, length));
    }
    else if (converterType == UCNV_UTF16_LittleEndian && !U_IS_BIG_ENDIAN && (length % sizeof(char16_t)) == 0) {
        const auto outLen = length / sizeof(char16_t);
        npc(out)->resize(outLen, 0);
        npc(out)->assign((const char16_t*)str, outLen);
    }
    else {
        auto status = U_ZERO_ERROR;
        char converterBuffer[U_CNV_SAFECLONE_BUFFERSIZE];
        int32_t converterLength = sizeof(converterBuffer);
        auto localConverter = ucnv_safeClone(converter, converterBuffer, &converterLength, &status);
        morphuntion::exception::ICUException::throwOnFailure(status, u"Could not clone converter");

        auto inputLength = length;

        npc(out)->resize(length, 0);
        // attempt to convert with a best guess buffer length.
        // if necessary, increase buffer and re-run
        for (int32_t i = 0; i < 2; ++i) {
            status = U_ZERO_ERROR;
            length = ucnv_toUChars(localConverter, (UChar *) &((*out)[0]), length, str,
                                   inputLength, &status);
            npc(out)->resize(length, 0);
            if (status != U_BUFFER_OVERFLOW_ERROR) {
                break;
            }
            // length got updated with the correct size, so redo
            // This is normally impossible to hit.
        }

        ucnv_close(localConverter);
        morphuntion::exception::ICUException::throwOnFailure(status, u"Conversion failed");
    }
}

} // namespace morphuntion::dictionary::metadata
