#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
cmake_minimum_required(VERSION 3.24)
get_version_from_makefile(MARISA_VERSION)
inflection_debug_vars(MARISA_VERSION)

set(MARISA_SOURCES
        trie.cc
        agent.cc
        grimoire/io/reader.cc
        grimoire/io/writer.cc
        grimoire/io/mapper.cc
        grimoire/trie/louds-trie.cc
        grimoire/trie/tail.cc
        grimoire/vector/bit-vector.cc
        keyset.cc
)

if(DEFINED ENV{MARISA_ROOT})
    message("Building with local copy of Marisa. MARISA_ROOT: " $ENV{MARISA_ROOT})
    list(TRANSFORM MARISA_SOURCES PREPEND ${MARISA_ROOT}/marisa-trie/lib/marisa/)
    set(PRIVATE_HEADERS_PATH ${MARISA_ROOT}/lib)
    set(PUBLIC_HEADERS_PATH ${MARISA_ROOT}/include)
else()

    set(ARTIFACT "v${MARISA_VERSION}.tar.gz")
    ExternalProject_Add(marisa-trie-download
            URL https://github.com/s-yata/marisa-trie/archive/refs/tags/v${MARISA_VERSION}.tar.gz
            BUILD_COMMAND ""
            CONFIGURE_COMMAND "" INSTALL_COMMAND "" LOG_DOWNLOAD ON EXCLUDE_FROM_ALL TRUE
            SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/marisa-trie
    )
    list(TRANSFORM MARISA_SOURCES PREPEND ${CMAKE_CURRENT_BINARY_DIR}/marisa-trie/lib/marisa/)
    set(PRIVATE_HEADERS_PATH ${CMAKE_CURRENT_BINARY_DIR}/marisa-trie/lib)
    set(PUBLIC_HEADERS_PATH ${CMAKE_CURRENT_BINARY_DIR}/marisa-trie/include)
    foreach (SOURCE IN LISTS MARISA_SOURCES)
        add_custom_command(OUTPUT ${SOURCE} DEPENDS marisa-trie-download)
    endforeach ()

endif() # (DEFINED ENV{MARISA_ROOT})


if(XCODE)
    add_library(marisa_objs INTERFACE)
    target_sources(marisa_objs INTERFACE ${MARISA_SOURCES})
    target_include_directories(marisa_objs INTERFACE ${PRIVATE_HEADERS_PATH} ${PUBLIC_HEADERS_PATH})
else()
    message("Configured with copy of Marisa in: " ${PUBLIC_HEADERS_PATH})
    add_library(marisa_objs OBJECT)
    set_property(TARGET marisa_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_sources(marisa_objs PRIVATE ${MARISA_SOURCES})
    target_include_directories(marisa_objs PRIVATE ${PRIVATE_HEADERS_PATH} PUBLIC ${PUBLIC_HEADERS_PATH})
endif()
