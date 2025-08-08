/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/ml/fwd.hpp>  
#include <inflection/util/fwd.hpp>                   
#include <inflection/tokenizer/TokenExtractor.hpp>   
#include <map>

class inflection::tokenizer::locale::ml::MlTokenizer final
{
public:
    static ::inflection::tokenizer::TokenExtractor* createTokenExtractor(
        const ::inflection::util::ULocale& locale,
        const ::std::map<::std::u16string_view, const char16_t*>& config);
};
