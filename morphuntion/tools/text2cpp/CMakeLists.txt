#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE TEXT2CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(text2cpp ${TEXT2CPP_SOURCES})
add_dependencies(tools text2cpp)