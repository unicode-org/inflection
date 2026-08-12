/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <string_view>

class inflection::tokenizer::ControlCleaver final
{
public:
    static void modifyTokens(const ::inflection::tokenizer::Tokenizer* tokenizer, std::u16string_view originalInput, inflection::tokenizer::Token_Head* head);

private:
    ControlCleaver() = delete;
};
