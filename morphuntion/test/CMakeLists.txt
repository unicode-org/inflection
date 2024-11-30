#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp")
add_executable(mtest ${TEST_SOURCES})

target_link_libraries(mtest
        PRIVATE
            morphuntion
            Catch2
            xml2
            marisa_objs
            CoreFoundation
            ICU::uc ICU::i18n
            $<$<PLATFORM_ID:Darwin>:${PERFDATA_FRAMEWORK}>
            $<$<PLATFORM_ID:Linux>:pthread>
)
target_link_options(mtest PRIVATE $<TARGET_PROPERTY:morphuntion,LINK_OPTIONS>)
target_include_directories(mtest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_BINARY_DIR}/resources/
            $<$<PLATFORM_ID:Darwin>:${PERFDATA_FRAMEWORK}>
)
fixRuntimePath(mtest morphuntion ../src)
add_compile_definitions(mtest MORPHUNTION_ROOT=\"${MORPHUNTION_DATA_ROOT_PREFIX}\")
add_dependencies(mtest morphuntion-data)

# Runs morphuntion unit tests: "make check"
add_custom_target(check
        DEPENDS mtest
        COMMAND ${CMAKE_COMMAND} -DDYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH} -DEXECUTABLE=$<TARGET_FILE:mtest> -P ${CMAKE_SOURCE_DIR}/cmake/runTests.cmake
        COMMENT "Running morphuntion tests"
        VERBATIM
)
#End section