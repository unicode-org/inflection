/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/da/DaTokenizer.hpp>

#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>
#include <inflection/tokenizer/locale/da/DaDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace da {

::inflection::tokenizer::TokenExtractor* DaTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor(locale, config, DaDictionaryTokenizerConfig());
}

} // namespace tokenizer::locale::da
} // namespace inflection
