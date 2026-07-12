#
# Copyright 2022-2025 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE BUILD_STRING_MAP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(buildStringMap ${BUILD_STRING_MAP_SOURCES})
target_link_libraries(buildStringMap
        PRIVATE
            tool_libraries
            marisa_objs
            inflection_tool_objs
            resource_objs
)

add_dependencies(tools buildStringMap)
