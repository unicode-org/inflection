/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/locale/DefaultTokenizer.hpp>

#include <morphuntion/tokenizer/TokenExtractor.hpp>

namespace morphuntion::tokenizer::locale {

::morphuntion::tokenizer::TokenExtractor* DefaultTokenizer::createTokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::morphuntion::tokenizer::TokenExtractor(locale, config);
}

} // namespace morphuntion::tokenizer::locale
