/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/sv/SvTokenizer.hpp>

#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>
#include <inflection/tokenizer/locale/sv/SvDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace sv {


::inflection::tokenizer::TokenExtractor* SvTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor(locale, config, SvDictionaryTokenizerConfig());
}

} // namespace tokenizer::locale::sv
} // namespace inflection
