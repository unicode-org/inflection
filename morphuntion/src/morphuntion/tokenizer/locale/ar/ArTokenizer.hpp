/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/locale/ar/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/tokenizer/TokenExtractor.hpp>
#include <map>

class morphuntion::tokenizer::locale::ar::ArTokenizer final
{
public:
    static ::morphuntion::tokenizer::TokenExtractor* createTokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
};
