/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiWordAndDelimiterTokenExtractor.hpp>

#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/locale/fi/FiTokenExtractorIterator.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>

namespace inflection::tokenizer::locale {
namespace fi {

FiWordAndDelimiterTokenExtractor::FiWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
    : super(locale, config)
    , corpus(getRequiredKeyAsPath(locale, config, u"tokenizer.dictionary.file"))
{
}

iterator::TokenExtractorIterator* FiWordAndDelimiterTokenExtractor::createIterator(std::u16string_view str) const
{
    return new FiTokenExtractorIterator(corpus, str);
}

} // namespace tokenizer::locale::fi
} // namespace inflection
