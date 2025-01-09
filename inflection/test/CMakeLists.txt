#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp")
add_executable(itest ${TEST_SOURCES})

target_link_libraries(itest
        PRIVATE
            inflection
            Catch2
            xml2
            marisa_objs
            CoreFoundation
            ICU::uc ICU::i18n
            $<$<PLATFORM_ID:Darwin>:${PERFDATA_FRAMEWORK}>
            $<$<PLATFORM_ID:Linux>:pthread>
)
target_link_options(itest PRIVATE $<TARGET_PROPERTY:inflection,LINK_OPTIONS>)
target_include_directories(itest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_BINARY_DIR}/resources/
            $<$<PLATFORM_ID:Darwin>:${PERFDATA_FRAMEWORK}>
)
fixRuntimePath(itest inflection ../src)
add_compile_definitions(itest INFLECTION_ROOT=\"${INFLECTION_DATA_ROOT_PREFIX}\")
add_dependencies(itest inflection-data)

# Runs inflection unit tests: "make check"
add_custom_target(check
        DEPENDS itest
        COMMAND ${CMAKE_COMMAND} -DDYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH} -DEXECUTABLE=$<TARGET_FILE:itest> -P ${CMAKE_SOURCE_DIR}/cmake/runTests.cmake
        COMMENT "Running inflection tests"
        VERBATIM
)
#End section