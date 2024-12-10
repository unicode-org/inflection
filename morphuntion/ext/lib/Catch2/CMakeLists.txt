#
# Copyright 2024-2024 Apple Inc. All rights reserved.
#
cmake_minimum_required(VERSION 3.24)
get_version_from_makefile(CATCH2_VERSION)
morphuntion_debug_vars(CATCH2_VERSION)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.tar.gz
)

FetchContent_MakeAvailable(Catch2)
