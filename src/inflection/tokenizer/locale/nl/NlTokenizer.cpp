/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/nl/NlTokenizer.hpp>

#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>
#include <inflection/tokenizer/locale/nl/NlDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace nl {

::inflection::tokenizer::TokenExtractor* NlTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor(locale, config, NlDictionaryTokenizerConfig());
}

} // namespace tokenizer::locale::nl
} // namespace inflection
