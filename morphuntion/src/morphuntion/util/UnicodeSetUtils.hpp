/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/util/Validate.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <string>
#include <string_view>

class morphuntion::util::UnicodeSetUtils final
{
public:
    /**
     * Equivalent to uset_containsSomeCodePoints
     */
    static bool containsSome(const ::icu4cxx::UnicodeSet &set, std::u16string_view string);
    static const ::icu4cxx::UnicodeSet *freeze(::icu4cxx::UnicodeSet *set);
    static ::icu4cxx::UnicodeSet *closeOver(::icu4cxx::UnicodeSet *set, int32_t attributes);

    static ::icu4cxx::UnicodeSet *add(::icu4cxx::UnicodeSet *set, char32_t c);
    static ::icu4cxx::UnicodeSet *removeAll(::icu4cxx::UnicodeSet *set, const ::icu4cxx::UnicodeSet & removeSet);

    static ::std::u16string* removeSetFromString(::std::u16string* dest, const ::icu4cxx::UnicodeSet &setToRemove, std::u16string_view str);
    static ::std::u16string* replaceSetFromString(::std::u16string* dest, const ::icu4cxx::UnicodeSet &setToMatch, std::u16string_view str, std::u16string_view replacement);
private:
    UnicodeSetUtils() = delete;
};

inline
const ::icu4cxx::UnicodeSet *
morphuntion::util::UnicodeSetUtils::freeze(::icu4cxx::UnicodeSet *set) {
    ::morphuntion::util::Validate::notNull(set)->freeze();
    return set;
}

inline
::icu4cxx::UnicodeSet *
morphuntion::util::UnicodeSetUtils::closeOver(::icu4cxx::UnicodeSet *set, int32_t attributes) {
    ::morphuntion::util::Validate::notNull(set)->closeOver(attributes);
    return set;
}

inline
::icu4cxx::UnicodeSet *
morphuntion::util::UnicodeSetUtils::add(::icu4cxx::UnicodeSet *set, char32_t c) {
    ::morphuntion::util::Validate::notNull(set)->add(c);
    return set;
}

inline
::icu4cxx::UnicodeSet *
morphuntion::util::UnicodeSetUtils::removeAll(::icu4cxx::UnicodeSet *set, const ::icu4cxx::UnicodeSet& removeSet) {
    ::morphuntion::util::Validate::notNull(set)->removeAll(removeSet);
    return set;
}