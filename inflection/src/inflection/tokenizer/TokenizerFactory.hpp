/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <map>
#include <string_view>

/**
 * @brief Creates Tokenizers for a give locale.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::TokenizerFactory final
{
public:
    /**
     * Create the language specific tokenizer.
     */
    static Tokenizer* createTokenizer(const ::inflection::util::ULocale& locale);

private:
    static Tokenizer* createTokenizerObject(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& systemConfig);

    TokenizerFactory() = delete;
};
