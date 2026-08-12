/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/Object.hpp>
#include <string>
#include <string_view>
#include <unicode/ucnv.h>

class INFLECTION_INTERNAL_API inflection::dictionary::metadata::CharsetConverter
    : public virtual ::inflection::Object
{
public:
    explicit CharsetConverter(const char* encoding);

    void encode(::std::string* out, std::u16string_view str) const;
    void decode(::std::u16string* out, const char* str, int32_t length) const;
    ~CharsetConverter() override;

private:
    UConverter *converter {  };
    UConverterType converterType {  };

    CharsetConverter(CharsetConverter&) = delete;
    CharsetConverter& operator=(const CharsetConverter&) = delete;
};

