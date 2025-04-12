#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
add_custom_target(tools)
add_library(tool_libraries INTERFACE)

if(CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
endif()

set(CMAKE_CXX_FLAGS_${BUILD_TYPE} ${CMAKE_CXX_FLAGS_RELEASE})
target_link_libraries(tool_libraries
        INTERFACE
            xml2
            ICU::uc ICU::i18n
            $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
)

add_subdirectory(buildDictionary)
add_subdirectory(buildTokDictionary)
add_subdirectory(text2cpp)
add_subdirectory(xml2cpp)