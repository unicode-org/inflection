/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <string_view>

/**
 * @brief Creates Tokenizers for a give locale.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::TokenizerFactory final
{
public:
    /**
     * Create the language specific tokenizer.
     */
    static Tokenizer* createTokenizer(const ::morphuntion::util::ULocale& locale);

private:
    static Tokenizer* createTokenizerObject(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& systemConfig);

    TokenizerFactory() = delete;
};
