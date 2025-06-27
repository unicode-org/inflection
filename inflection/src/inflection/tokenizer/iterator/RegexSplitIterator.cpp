/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/iterator/RegexSplitIterator.hpp>

#include <inflection/exception/ICUException.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <string_view>

namespace inflection::tokenizer::iterator {

RegexSplitIterator::RegexSplitIterator(const TokenExtractor& tokenExtractor, std::u16string_view str, TokenExtractorIterator& tokenIterator)
    : super()
    , tokenExtractor(tokenExtractor)
    , str(str)
    , tokenIterator(tokenIterator)
{
    if (tokenExtractor.splitPattern != nullptr) {
        matcher = *tokenExtractor.splitPattern;
    }
    reset(str);
}

RegexSplitIterator::~RegexSplitIterator()
{
}

int32_t RegexSplitIterator::getNextRegexBoundary()
{
    for (; group <= groupCount; group++) {
        int32_t newEnd = matcher.end(group) + start;
        if (start < newEnd && newEnd < end && previousBreak < newEnd) {
            previousBreak = newEnd;
            return newEnd;
        }
        // else an empty match or a whole match. Ignore it.
    }
    previousBreak = end;
    return end;
}

int32_t RegexSplitIterator::getNextBoundary()
{
    if (previousBreak < end) {
        return getNextRegexBoundary();
    }
    start = end;
    end = tokenIterator.getNextBoundary();
    auto length = end - start;
    if (length >= 2) {
        auto ec = U_ZERO_ERROR;
        utext_openUChars(&text, (const UChar *)str.data() + start, length, &ec);
        ::inflection::exception::ICUException::throwOnFailure(ec);
        matcher.setUText(&text);
        auto matchFound = matcher.find(0);
        if (matchFound) {
            if (tokenExtractor.wordsToNotSplit != nullptr) {
                ::inflection::util::StringViewUtils::lowercase(&lowercase, str.substr(start, length), tokenExtractor.locale);
                if (tokenExtractor.wordsToNotSplit->find(lowercase) != tokenExtractor.wordsToNotSplit->end()) {
                    goto exit;
                }
            }
            groupCount = matcher.groupCount();
            group = 1; // 0 is the whole regex
            return getNextRegexBoundary();
        }
        // Else don't modify this range.
    }
    // Else don't modify this range.

exit:
    previousBreak = end;
    return end;
}

void RegexSplitIterator::reset(std::u16string_view str)
{
    this->str = str;
    start = UBRK_DONE;
    previousBreak = 0;
    end = 0;
    group = -1;
    groupCount = -1;
}
} // namespace inflection::tokenizer
