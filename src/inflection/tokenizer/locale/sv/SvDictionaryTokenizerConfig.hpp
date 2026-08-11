/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/sv/fwd.hpp>
#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>

class inflection::tokenizer::locale::sv::SvDictionaryTokenizerConfig final
    : public ::inflection::tokenizer::dictionary::DictionaryTokenizerConfig
{
public:
    typedef ::inflection::tokenizer::dictionary::DictionaryTokenizerConfig super;

public:
    SvDictionaryTokenizerConfig();
};
