#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE XML2CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(xml2cpp ${XML2CPP_SOURCES})
target_link_libraries(xml2cpp LibXml2::LibXml2)
add_dependencies(tools xml2cpp)