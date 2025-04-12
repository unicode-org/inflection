#
# Copyright 2018-2024 Apple Inc. All rights reserved.
#
cmake_minimum_required(VERSION 3.24)
include(ExternalProject)
include(CheckCXXCompilerFlag)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(VERSIONS_MK_PATH ${CMAKE_SOURCE_DIR}/cmake/query_versions.mk)
set(OPTIONS_MK_PATH ${CMAKE_SOURCE_DIR}/cmake/build_options.mk)
include(inflectionMacros)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INSTALL_MESSAGE NEVER)

# Declare inflection project
project(
    Inflection
    LANGUAGES C CXX
)

# Configure number of processors
get_num_processors(NUM_PROCESSORS_VAL)
inflection_debug_vars(NUM_PROCESSORS_VAL)

# Unicode Inflection cache variables
set(NUM_PROCESSORS             ${NUM_PROCESSORS_VAL} CACHE STRING   "Number of cores to be used in make")

option(PROFILING "Turn on code profiling" OFF)

add_compile_options(${CXX_STD_LIB_FLAG})
add_link_options(${CXX_STD_LIB_FLAG})

# Setting c++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Equivalent to -fvisibility=hidden flag
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Unicode Inflection version
set(TAG_PREFIX Inflection)
get_inflection_version(INFLECTION_VERSION_TAG)
set(INFLECTION_VERSION ${INFLECTION_VERSION_TAG} CACHE STRING "Version of inflection to be used in publishing")
inflection_debug_vars(INFLECTION_VERSION)

set(CMAKE_INSTALL_LIBDIR lib)
include(GNUInstallDirs)

# Optionally compile with code profiling
if(PROFILING)
    message("-- PROFILING TURNED ON")
    add_compile_options(-g -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-g -fprofile-instr-generate -fcoverage-mapping)
endif()

# Set these warning properties on a project level
add_compile_options(-Wall -Weffc++ -Wextra)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wglobal-constructors -Wexit-time-destructors -Wweak-vtables -Wvla-extension)
endif()

# Improve code security
add_compile_options(-Werror=format-security -fstack-protector-strong)
add_compile_definitions(_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE)

# Add link time optimization for release build types for macOS
if(APPLE AND ("${CMAKE_BUILD_TYPE}" MATCHES "MinSizeRel" OR "${CMAKE_BUILD_TYPE}" MATCHES "Release"))
    add_compile_options(-flto)
    add_link_options(-flto)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        # Improve code security
        add_compile_definitions(_FORTIFY_SOURCE=2)
    endif()
endif()

# Set Unicode Inflection include, data directories
set(INFLECTION_INCLUDE_ROOT ${CMAKE_BINARY_DIR}/inflection_headers)
set(INFLECTION_DATA_ROOT_PREFIX ${CMAKE_BINARY_DIR}/inflection_data)
set(INFLECTION_DATA_ROOT ${INFLECTION_DATA_ROOT_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})

file(MAKE_DIRECTORY ${INFLECTION_DATA_ROOT})
file(MAKE_DIRECTORY ${INFLECTION_INCLUDE_ROOT})

include(dependICU)

add_library(xml2 INTERFACE IMPORTED GLOBAL)
set_target_properties(xml2 PROPERTIES IMPORTED_LIBNAME xml2)
target_include_directories(xml2 INTERFACE ${CMAKE_OSX_SYSROOT}/usr/include/libxml2)

# Runs Unicode Inflection unit tests: "make check"
set(DYLD_LIBRARY_PATH ${ICU_LIB_DIRECTORY}:$<TARGET_FILE_DIR:inflection>)

add_subdirectory(ext EXCLUDE_FROM_ALL)
add_subdirectory(tools EXCLUDE_FROM_ALL)
add_subdirectory(resources)
add_subdirectory(src)
add_subdirectory(test EXCLUDE_FROM_ALL)

add_custom_target(dist
    COMMAND sh -c "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/dist ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)
add_dependencies(dist inflection inflection-headers inflection-data)

add_subdirectory(docs EXCLUDE_FROM_ALL)

# make list-commands
add_custom_target(list-commands
        COMMAND echo "\
make list-commands          : Shows this message\\n\\n\
make check                  : Runs unit tests\\n\
make check-headers          : Tests whether all exported headers can be compiled independently.\\n\
make inflection             : Builds the shared library.\\n\
make inflection-headers     : Copy all inflection public headers to <build>/inflection_headers.\\n\
make inflection-data        : Generate all inflection data under <build>/inflection_data.\\n\
make dist                   : Builds inflection, the headers, and the data.\\n\
make coverage               : Generates code coverage using sonar-scanner\\n\
make generate-coverage-csv  : Generates code coverage as a csv\\n\
        "
        VERBATIM
)
# end section

install(TARGETS   inflection                  LIBRARY      COMPONENT inflection_library)
install(DIRECTORY ${INFLECTION_INCLUDE_ROOT}/ TYPE INCLUDE COMPONENT inflection_headers)
install(DIRECTORY ${INFLECTION_DATA_ROOT}/    TYPE DATA    COMPONENT inflection_data)
