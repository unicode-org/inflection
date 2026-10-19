/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/locale/de/fwd.hpp>

class inflection::tokenizer::locale::de::DeDictionaryTokenizerConfig final
    : public ::inflection::tokenizer::dictionary::DictionaryTokenizerConfig {

public:
    typedef ::inflection::tokenizer::dictionary::DictionaryTokenizerConfig super;

public:
    DeDictionaryTokenizerConfig();
};
