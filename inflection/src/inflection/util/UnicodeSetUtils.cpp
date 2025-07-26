/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::util {

bool
UnicodeSetUtils::containsSome(const ::icu4cxx::UnicodeSet &set, std::u16string_view string) {
    auto length = (int32_t) string.length();
    for (int32_t i = 0; i < length; ) {
        UChar32 cp;
        U16_NEXT(string, i, length, cp);
        if (set.contains(cp)) {
            return true;
        }
    }
    return false;
}

::std::u16string* UnicodeSetUtils::removeSetFromString(::std::u16string* dest, const ::icu4cxx::UnicodeSet &setToRemove, std::u16string_view str)
{
    return replaceSetFromString(dest, setToRemove, str, ::std::u16string_view());
}

::std::u16string* UnicodeSetUtils::replaceSetFromString(::std::u16string* dest, const ::icu4cxx::UnicodeSet &setToMatch, std::u16string_view str, std::u16string_view replacement)
{
    int32_t cp;
    int32_t strLen = int32_t(str.length());
    npc(dest)->clear();
    if (str.length() >= npc(dest)->capacity()) {
        npc(dest)->reserve(str.length());
    }
    for (int32_t i = 0; i < strLen; i += U16_LENGTH(cp)) {
        cp = StringViewUtils::codePointAt(str, i);
        if (setToMatch.contains(cp)) {
            if (!replacement.empty()) {
                npc(dest)->append(replacement);
            }
        } else {
            StringUtils::appendCodePoint(dest, cp);
        }
    }
    return dest;
}

} // namespace inflection::util
