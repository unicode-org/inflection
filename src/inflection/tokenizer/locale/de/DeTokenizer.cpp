/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/de/DeTokenizer.hpp>

#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>
#include <inflection/tokenizer/locale/de/DeDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace de {

::inflection::tokenizer::TokenExtractor* DeTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor(locale, config, DeDictionaryTokenizerConfig());
}

} // namespace tokenizer::locale::de
} // namespace inflection
