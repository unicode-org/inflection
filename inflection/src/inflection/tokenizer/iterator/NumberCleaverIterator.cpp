/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/iterator/NumberCleaverIterator.hpp>

#include <inflection/exception/ICUException.hpp>
#include <string_view>

namespace inflection::tokenizer::iterator {

NumberCleaverIterator::NumberCleaverIterator(std::u16string_view str, TokenExtractorIterator& tokenIterator)
    : super()
    , str(str)
    , tokenIterator(tokenIterator)
{
}

NumberCleaverIterator::~NumberCleaverIterator()
{
}

int32_t NumberCleaverIterator::getNextBoundary()
{
    if (split != UBRK_DONE) {
        split = UBRK_DONE;
        return end;
    }
    start = end;
    end = tokenIterator.getNextBoundary();
    auto length = end - start;
    if (length >= 2) {
        char32_t startChar;
        U16_GET(str, start, start, end, startChar);
        char32_t endChar;
        U16_GET(str, start, end - 1, end, endChar);
        auto startsDigit = u_isdigit(startChar);
        auto endsDigit = u_isdigit(endChar);
        if (startsDigit != endsDigit) {
            auto startsLetter = u_isalpha(startChar);
            auto endsLetter = u_isalpha(endChar);
            if (startsLetter != endsLetter) {
                char32_t splitChar;
                split = start;
                if (startsDigit) {
                    while (split < end) {
                        U16_GET(str, start, split, end, splitChar);
                        if (!(bool) u_ispunct(splitChar) && !(bool) u_isdigit(splitChar)) {
                            break;
                        }
                        split++;
                    }
                } else {
                    while (split < end) {
                        U16_GET(str, start, split, end, splitChar);
                        if (!(bool) u_isalpha(splitChar)) {
                            break;
                        }
                        split++;
                    }
                }
                U16_GET(str, start, split - (startsDigit ? 1 : 0), end, splitChar);
                if (!(bool) u_ispunct(splitChar)) {
                    // It's probably something like 9AM or HAL9000
                    return split;
                }
                // It's something like A.9 or something else. We're splitting.
                split = UBRK_DONE;
            }
        }
    }

    return end;
}

void NumberCleaverIterator::reset(std::u16string_view str)
{
    this->str = str;
    split = UBRK_DONE;
    start = end = 0;
}

} // namespace inflection::tokenizer
