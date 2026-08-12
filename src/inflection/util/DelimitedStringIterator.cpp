/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>

namespace inflection::util {

DelimitedStringIterator::DelimitedStringIterator(std::u16string_view str, std::u16string_view delimiterString)
    : delimiterString(delimiterString)
    , originalString(str)
    , length(static_cast<int32_t>(str.length()))
{
    if (hasNext()) {
        getNext();
    }
}

DelimitedStringIterator::~DelimitedStringIterator()
{
}

bool
DelimitedStringIterator::hasNext() const {
    return currentIndex < length || !currentString.empty();
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
    const auto firstOccurrence = originalString.find(delimiterString, currentIndex);
    if (firstOccurrence != std::u16string_view::npos) {
        currentString = originalString.substr(currentIndex, firstOccurrence - currentIndex);
        currentIndex = static_cast<int32_t>(firstOccurrence + delimiterString.length());
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
