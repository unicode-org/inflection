/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/locale/ar/ArTokenizer.hpp>

#include <morphuntion/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>

namespace morphuntion::tokenizer::locale::ar {

::morphuntion::tokenizer::TokenExtractor* ArTokenizer::createTokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    return new ::morphuntion::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor(locale, ::morphuntion::lang::StringFilterUtil::ARABIC_SCRIPT(), config);
}

} // namespace morphuntion::tokenizer::locale::ar
