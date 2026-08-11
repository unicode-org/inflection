/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/DefaultTokenizer.hpp>

#include <inflection/tokenizer/TokenExtractor.hpp>

namespace inflection::tokenizer::locale {

::inflection::tokenizer::TokenExtractor* DefaultTokenizer::createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::inflection::tokenizer::TokenExtractor(locale, config);
}

} // namespace inflection::tokenizer::locale
