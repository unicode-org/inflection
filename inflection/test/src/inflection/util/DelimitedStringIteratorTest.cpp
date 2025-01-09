/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/DelimitedStringIterator.hpp>

TEST_CASE("DelimitedStringIteratorTest#testNewline")
{
    ::std::u16string str(u"a\nb");
    ::inflection::util::DelimitedStringIterator iterator(str, u"\n");
    REQUIRE(iterator.hasNext());
    REQUIRE(*iterator == u"a");
    ++iterator;
    REQUIRE(iterator.hasNext());
    REQUIRE(*iterator == u"b");
    ++iterator;
    REQUIRE(!iterator.hasNext());
}