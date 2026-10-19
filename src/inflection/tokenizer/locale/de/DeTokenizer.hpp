/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/TokenExtractor.hpp>
#include <inflection/tokenizer/locale/de/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <map>

class inflection::tokenizer::locale::de::DeTokenizer final {
public:
    static ::inflection::tokenizer::TokenExtractor* createTokenExtractor(
        const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
};
