/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <cstdint>
#include <string_view>
#include <vector>

class inflection::tokenizer::locale::fi::FiSuffixSplit final
{
public: /* package */
    enum SuffixType {
        SUFFIX,
        MIDDLE_SUFFIX
    };

public:
    static bool isSuffix(std::u16string_view str);
    static void addSuffixInfo(std::u16string_view suffix, ::std::vector<int32_t>* indexes, int32_t offset);
    static bool isInNotNominative(const FiSegment* segment, std::u16string_view suffix);

private:
    static void stemming(::std::vector<::std::u16string_view>* result, std::u16string_view word, SuffixType suffixType);
    FiSegment* segment {  };
    float logProb {  };
    bool found_ {  };

public: /* package */
    bool found() const;
    float getLogProb() const;
    FiSegment* getSegment() const;
    int32_t getStart() const;
    int32_t getEnd() const;

public:
    void addBoundaries(::std::vector<int32_t>* indexes, SuffixType suffixType);

public: /* package */
    explicit FiSuffixSplit(FiSegment* segment);

    static bool containsSuffixSplit(::std::u16string_view suffix);
private:
    static bool containsMiddleSuffix(::std::u16string_view suffix);
    static bool containsClitic(::std::u16string_view suffix);
};
