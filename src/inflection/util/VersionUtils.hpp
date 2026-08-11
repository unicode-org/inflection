/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>

#include <inflection/util/fwd.hpp>

/**
 * @brief Runtime version information about Inflection.
 */
class INFLECTION_CLASS_API inflection::util::VersionUtils
{
public:
    /**
     * The version of the library linked during runtime. This may be a different version than the headers used at compile time.
     */
    static const char* getLibraryVersion();
};
