/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <inflection/Object.hpp>

class inflection::tokenizer::locale::fi::FiTokenizerConfig final
{
public: /* package */
    static constexpr int32_t MIN_FISUFFIXSPLIT { int32_t(6) };
    static constexpr int32_t MIN_FICOMPOUNDSPLIT { int32_t(7) };
    static constexpr int32_t MAXMIDDLESUFFIXLENGTH { int32_t(3) };
    static constexpr int32_t MINUNKNOWNSTEMLENGTH { int32_t(5) };
    static constexpr int32_t MINSTEMLENGTH { int32_t(3) };
    static constexpr int32_t MAXSUFFIXLENGTH { int32_t(6) };
    static constexpr int32_t MAXCLITICLENGTH { int32_t(4) };
    static constexpr int32_t MINSUFFIXSPLIT { int32_t(4) };
    static constexpr float MINLOGPROB { -1000.0f };

private:
    FiTokenizerConfig() = delete;
};
