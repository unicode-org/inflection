/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/util/ResourceLocator.hpp>

class morphuntion::tokenizer::StringTokenizerBridgeProvider final
{
public:
    static constexpr auto TOKENIZER_DATA_DIR = u"/tokenizer";

private:
    StringTokenizerBridgeProvider() = delete;
};
