#
# Copyright 2025 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE BUILD_GENEXEMPLARS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(genExemplars ${BUILD_GENEXEMPLARS_SOURCES})
target_link_libraries(genExemplars
        PRIVATE
            tool_libraries
            marisa_objs
            inflection_tool_objs
            resource_objs
)

add_dependencies(tools genExemplars)
