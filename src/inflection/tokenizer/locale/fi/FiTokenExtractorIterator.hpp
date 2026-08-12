/*
 * Copyright 2021-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <string_view>
#include <vector>

class inflection::tokenizer::locale::fi::FiTokenExtractorIterator
    : public virtual ::inflection::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::inflection::tokenizer::iterator::TokenExtractorIterator super;
private:
    const inflection::tokenizer::trie::SerializedTrie& corpus;
    iterator::ICUTokenExtractorIterator tokenExtractorIterator;
    std::u16string_view charSequence {  };
    ::std::u16string sbWord {  };
    std::vector<int32_t> boundaries {  };
    int32_t boundariesIdx {  };
    int32_t lastStart {  };
public:

    /**
     * Return the end of the current token.
     */
    int32_t getNextBoundary() override;

    /**
     * Reset the iterator with a new string.
     * @param str The new string to iterator over.
     */
    void reset(std::u16string_view str) override;

public:
    FiTokenExtractorIterator(const inflection::tokenizer::trie::SerializedTrie& corpus, std::u16string_view str);
    ~FiTokenExtractorIterator() override;
private:
    FiTokenExtractorIterator(const FiTokenExtractorIterator&) = delete;
    FiTokenExtractorIterator& operator=(const FiTokenExtractorIterator&) = delete;
};
