/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/nb/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <map>

class inflection::tokenizer::locale::nb::NbTokenizer final
{
public:
    static ::inflection::tokenizer::TokenExtractor* createTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
};
