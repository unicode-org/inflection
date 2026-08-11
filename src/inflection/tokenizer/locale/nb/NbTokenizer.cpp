/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/nb/NbTokenizer.hpp>

#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>
#include <inflection/tokenizer/locale/nb/NbDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace nb {


::inflection::tokenizer::TokenExtractor* NbTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor(locale, config, NbDictionaryTokenizerConfig());
}

} // namespace tokenizer::locale::nb
} // namespace inflection
