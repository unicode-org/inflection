#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE BUILD_DICTIONARY_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(buildDictionary ${BUILD_DICTIONARY_SOURCES})
target_link_libraries(buildDictionary
        PRIVATE
            tool_libraries
            LibXml2::LibXml2
            marisa_objs
            inflection_tool_objs
            resource_objs
)

add_dependencies(tools buildDictionary)
