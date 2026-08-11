#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE BUILD_TOK_DICTIONARY_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(buildTokDictionary ${BUILD_TOK_DICTIONARY_SOURCES})
target_link_libraries(buildTokDictionary
        PRIVATE
            tool_libraries
            marisa_objs
            inflection_tool_objs
            resource_objs
)

add_dependencies(tools buildTokDictionary)
