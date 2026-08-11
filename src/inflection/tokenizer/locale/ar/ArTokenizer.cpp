/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/ar/ArTokenizer.hpp>

#include <inflection/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor.hpp>

namespace inflection::tokenizer::locale::ar {

::inflection::tokenizer::TokenExtractor* ArTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor(locale, USCRIPT_ARABIC, config);
}

} // namespace inflection::tokenizer::locale::ar
