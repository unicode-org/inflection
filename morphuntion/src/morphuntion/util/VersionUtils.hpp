/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/api.h>

#include <morphuntion/util/fwd.hpp>

/**
 * @brief Runtime version information about Morphuntion.
 */
class MORPHUNTION_CLASS_API morphuntion::util::VersionUtils
{
public:
    /**
     * The version of the library linked during runtime. This may be a different version than the headers used at compile time.
     */
    static const char* getLibraryVersion();
};
