/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/DictionaryMetaData.h>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/npc.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/util/ULocale.hpp>

using ::morphuntion::util::CFUtils;
using ::morphuntion::dictionary::DictionaryMetaData;

MORPHUNTION_CAPI int64_t* mddmd_getCombinedBinaryType(const MDDictionaryMetaData *thisObject, int64_t *result, CFStringRef word, CFErrorRef *error) {
    try {
        return npc((const DictionaryMetaData*)thisObject)->getCombinedBinaryType(result, ::std::u16string_view(CFUtils::to_u16string(word)));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFArrayRef mddmd_createPropertyNames(const MDDictionaryMetaData *thisObject, int64_t binaryProperties, CFErrorRef *error) {
    try {
        return CFUtils::to_CFArray(npc((const DictionaryMetaData*)thisObject)->getPropertyNames(binaryProperties));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef mddmd_createPropertyName(const MDDictionaryMetaData *thisObject, int64_t singleProperty, CFErrorRef *error) {
    try {
        return CFUtils::to_CFString(npc((const DictionaryMetaData*)thisObject)->getPropertyName(singleProperty));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI bool mddmd_isKnownWord(const MDDictionaryMetaData *thisObject, CFStringRef word, CFErrorRef *error) {
    try {
        return npc((const DictionaryMetaData*)thisObject)->isKnownWord(::std::u16string_view(CFUtils::to_u16string(word)));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return false;
}

MORPHUNTION_CAPI int64_t mddmd_getBinaryProperties(const MDDictionaryMetaData *thisObject, CFArrayRef properties, CFErrorRef *error) {
    try {
        return npc((const DictionaryMetaData*)thisObject)->getBinaryProperties(CFUtils::to_u16stringVector(properties));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return 0;
}

MORPHUNTION_CAPI const MDDictionaryMetaData* mddmd_getDictionary(CFLocaleRef locale, CFErrorRef* error) {
    try {
        return (const MDDictionaryMetaData*) npc(DictionaryMetaData::createDictionary(morphuntion::util::CFUtils::convert(locale)));
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return nullptr;
}
