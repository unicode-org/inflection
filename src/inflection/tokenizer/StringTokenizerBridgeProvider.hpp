/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/util/ResourceLocator.hpp>

class inflection::tokenizer::StringTokenizerBridgeProvider final
{
public:
    static constexpr auto TOKENIZER_DATA_DIR = u"/tokenizer";

private:
    StringTokenizerBridgeProvider() = delete;
};
