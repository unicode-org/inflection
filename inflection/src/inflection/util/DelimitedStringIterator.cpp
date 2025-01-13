/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <unicode/ustring.h>

namespace inflection::util {


DelimitedStringIterator::DelimitedStringIterator(std::u16string_view str, std::u16string_view delimiterString)
    : delimiterString(delimiterString)
    , originalString(str)
    , length(int32_t(str.length()))
{
    if (hasNext()) {
        getNext();
    }
}

DelimitedStringIterator::DelimitedStringIterator(const DelimitedStringIterator& other)
    : delimiterString(other.delimiterString)
    , originalString(other.originalString)
    , length(other.length)
    , currentIndex(other.currentIndex)
{
}

DelimitedStringIterator::~DelimitedStringIterator()
{
}

bool
DelimitedStringIterator::hasNext() const {
    return currentIndex < length || currentString.length() != 0;
}

std::u16string_view
DelimitedStringIterator::operator*() const {
    return currentString;
}

void
DelimitedStringIterator::getNext()
{
    if (currentIndex >= length) {
        currentString = u"";
        return;
    }
    auto strStart = originalString.data() + currentIndex;
    const char16_t* result = (const char16_t *)u_strFindFirst((const UChar *)strStart, length - currentIndex, (const UChar *)delimiterString.data(), int32_t(delimiterString.length()));
    if (result != nullptr) {
        currentString = originalString.substr(currentIndex, (uint32_t)(result - strStart));
        currentIndex = int32_t(result - originalString.data() + delimiterString.length());
    }
    else {
        currentString = originalString.substr(currentIndex, length - currentIndex);
        currentIndex = length;
    }
}

DelimitedStringIterator&
DelimitedStringIterator::operator++() {
    if (!hasNext()) {
        throw ::inflection::exception::IndexOutOfBoundsException();
    }
    getNext();
    return *this;
}

} // namespace inflection::util
