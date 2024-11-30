/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/resources/fwd.hpp>
#include <map>
#include <string_view>

class morphuntion::resources::DataResource final
{
public:
    static ::std::map<::std::u16string_view, const char16_t*> getProperties(::std::u16string_view resourceName, bool returnEmptyOnFailure = false);
    static const char16_t *getString(::std::u16string_view resourceName, bool returnNullOnFailure = false);

private:
    DataResource() = delete;
};
