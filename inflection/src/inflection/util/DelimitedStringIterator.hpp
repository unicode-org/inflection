/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <string>
#include <string_view>

class INFLECTION_INTERNAL_API inflection::util::DelimitedStringIterator
{
public:

    bool hasNext() const;

    std::u16string_view     operator*() const;
    DelimitedStringIterator&    operator++();

    DelimitedStringIterator(std::u16string_view str, std::u16string_view delimiterString);
    DelimitedStringIterator(const DelimitedStringIterator& other);
    ~DelimitedStringIterator();

private:
    void getNext();

    std::u16string_view currentString {  };
    std::u16string_view delimiterString {  };
    std::u16string_view originalString {  };
    int32_t length {  };
    int32_t currentIndex {  };

    DelimitedStringIterator& operator=(const DelimitedStringIterator&) = delete;
};
