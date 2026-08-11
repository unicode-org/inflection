/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryMetaData.h>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/npc.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <unicode/ustring.h>

using ::inflection::util::TypeConversionUtils;
using ::inflection::dictionary::DictionaryMetaData;

INFLECTION_CAPI int64_t* iddmd_getCombinedBinaryType(const IDDictionaryMetaData *thisObject, int64_t *result, const char16_t* word, int32_t wordLen, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(word);
            return npc((const DictionaryMetaData*)thisObject)->getCombinedBinaryType(result,
                                                                                     wordLen < 0
                                                                                     ? ::std::u16string_view(word)
                                                                                     : ::std::u16string_view(word, wordLen));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI int32_t iddmd_getPropertyName(const IDDictionaryMetaData *thisObject, int64_t singleProperty, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(dest);
            auto result(npc((const DictionaryMetaData*)thisObject)->getPropertyName(singleProperty));
            auto resultLen = int32_t(result.length());
            if (destCapacity > 0 && resultLen < destCapacity) {
                u_strncpy((UChar*)dest, (const UChar*)result.c_str(), resultLen);
            }
            return inflection::util::TypeConversionUtils::terminateString(dest, destCapacity, resultLen, status);
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI bool iddmd_isKnownWord(const IDDictionaryMetaData *thisObject, const char16_t* word, int32_t wordLen, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(word);
            return npc((const DictionaryMetaData*)thisObject)->isKnownWord(wordLen < 0
                                                                           ? ::std::u16string_view(word)
                                                                           : ::std::u16string_view(word, wordLen));
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI int64_t iddmd_getBinaryProperties(const IDDictionaryMetaData *thisObject, const char16_t* const properties[], int32_t propertiesLength, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(properties);
            inflection::util::Validate::isTrue(propertiesLength >= 0);
            ::std::vector<::std::u16string> propertiesVec;
            propertiesVec.reserve(propertiesLength);
            for (int32_t propIdx = 0; propIdx < propertiesLength; propIdx++) {
                propertiesVec.emplace_back(npc(properties[propIdx]));
            }
            return npc((const DictionaryMetaData*)thisObject)->getBinaryProperties(propertiesVec);
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
