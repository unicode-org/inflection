/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <string_view>

class inflection::tokenizer::locale::fi::FiWordAndDelimiterTokenExtractor final
    : public ::inflection::tokenizer::TokenExtractor
{
public:
    typedef ::inflection::tokenizer::TokenExtractor super;

private:
    ::inflection::tokenizer::trie::SerializedTrie corpus;

public: /* protected */
    iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const override;

public: /* package */
    FiWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
};
