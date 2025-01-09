/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryMetaData.h>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/npc.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>

using ::inflection::util::TypeConversionUtils;
using ::inflection::dictionary::DictionaryMetaData;

INFLECTION_CAPI int64_t* iddmd_getCombinedBinaryType(const IDDictionaryMetaData *thisObject, int64_t *result, CFStringRef word, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((const DictionaryMetaData*)thisObject)->getCombinedBinaryType(result, ::std::u16string_view(TypeConversionUtils::to_u16string(word)));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFArrayRef iddmd_createPropertyNames(const IDDictionaryMetaData *thisObject, int64_t binaryProperties, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return TypeConversionUtils::to_CFArray(npc((const DictionaryMetaData*)thisObject)->getPropertyNames(binaryProperties));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFStringRef iddmd_createPropertyName(const IDDictionaryMetaData *thisObject, int64_t singleProperty, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return TypeConversionUtils::to_CFString(npc((const DictionaryMetaData*)thisObject)->getPropertyName(singleProperty));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI bool iddmd_isKnownWord(const IDDictionaryMetaData *thisObject, CFStringRef word, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((const DictionaryMetaData*)thisObject)->isKnownWord(::std::u16string_view(TypeConversionUtils::to_u16string(word)));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI int64_t iddmd_getBinaryProperties(const IDDictionaryMetaData *thisObject, CFArrayRef properties, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((const DictionaryMetaData*)thisObject)->getBinaryProperties(TypeConversionUtils::to_u16stringVector(properties));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return 0;
}

INFLECTION_CAPI const IDDictionaryMetaData* iddmd_getDictionary(const char* locale, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (const IDDictionaryMetaData*) npc(DictionaryMetaData::createDictionary(inflection::util::ULocale(npc(locale))));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}
