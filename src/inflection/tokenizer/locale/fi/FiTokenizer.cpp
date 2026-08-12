/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiTokenizer.hpp>

#include <inflection/tokenizer/locale/fi/FiWordAndDelimiterTokenExtractor.hpp>

namespace inflection::tokenizer::locale {
namespace fi {

::inflection::tokenizer::TokenExtractor* FiTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new FiWordAndDelimiterTokenExtractor(locale, config);
}

} // namespace tokenizer::locale::fi
} // namespace inflection
