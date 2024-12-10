/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/metadata/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <string>
#include <string_view>
#include <unicode/ucnv.h>

class MORPHUNTION_INTERNAL_API morphuntion::dictionary::metadata::CharsetConverter
    : public virtual ::morphuntion::Object
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

