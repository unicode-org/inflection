#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
set(RESOURCE_GRAMMAR_DATA_HPP ${CMAKE_BINARY_DIR}/resources/inflection/lang/features/LanguageGrammarFeatures_Data.hpp)
set(RESOURCE_VERSION_H ${CMAKE_BINARY_DIR}/resources/inflection/version.h)

file(GLOB_RECURSE RESOURCE_PROPS ${CMAKE_CURRENT_SOURCE_DIR}/org/*.properties)
file(GLOB_RECURSE RESOURCE_DICTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/org/*)
foreach (FILTER_REGEX IN ITEMS "\\.properties$" "\\.dtd$" "\\.xml$" "\\.lst$" "tokenizer\\.dictionary" "CMakeLists\\.txt" "/\\.")
    list(FILTER RESOURCE_DICTS EXCLUDE REGEX ${FILTER_REGEX})
endforeach ()

list(TRANSFORM RESOURCE_DICTS APPEND ",string" OUTPUT_VARIABLE RESOURCE_DICTS_WITH_TYPE)
list(TRANSFORM RESOURCE_PROPS APPEND ",properties" OUTPUT_VARIABLE RESOURCE_PROPS_WITH_TYPE)
list(APPEND RESOURCES_HPP ${RESOURCE_VERSION_H})

foreach (RESOURCE_ITEM IN LISTS RESOURCE_DICTS_WITH_TYPE RESOURCE_PROPS_WITH_TYPE)

    string(REPLACE "," ";" RESOURCE_ITEM_LIST ${RESOURCE_ITEM})
    list(GET RESOURCE_ITEM_LIST 0 RESOURCE_FILE)
    list(GET RESOURCE_ITEM_LIST 1 RESOURCE_TYPE)

    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}/" "" RESOURCE_RELATIVE_PATH ${RESOURCE_FILE})
    string(REGEX REPLACE "\\.properties$" "" RESOURCE_RELATIVE_PATH ${RESOURCE_RELATIVE_PATH})

    string(REGEX REPLACE "/" "::" RESOURCE_CLASS ${RESOURCE_RELATIVE_PATH})
    set(RESOURCE_HPP ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_RELATIVE_PATH}.hpp)
    get_filename_component(RESOURCE_CPP_DIR ${RESOURCE_HPP} DIRECTORY)

    list(APPEND RESOURCE_DIRS ${RESOURCE_CPP_DIR})
    list(APPEND RESOURCES_HPP ${RESOURCE_HPP})

    add_custom_command(
        OUTPUT ${RESOURCE_HPP}
        COMMAND text2cpp --infile ${RESOURCE_FILE} --outfile ${RESOURCE_HPP} --class_name ${RESOURCE_CLASS} --type ${RESOURCE_TYPE}
        DEPENDS text2cpp ${RESOURCE_FILE}
    )
endforeach ()

set(DATA_RESOURCE_CPP ${CMAKE_CURRENT_BINARY_DIR}/inflection/resources/DataResource.cpp)
set(DATA_RESOURCE_GEN ${CMAKE_SOURCE_DIR}/tools/text2cpp/Generate_DataResource.cpp.sh)
add_custom_command(
    OUTPUT ${DATA_RESOURCE_CPP}
    COMMAND ${DATA_RESOURCE_GEN} ${DATA_RESOURCE_CPP} ${CMAKE_SOURCE_DIR}
    DEPENDS ${DATA_RESOURCE_GEN} ${RESOURCES_HPP}
)
get_filename_component(DATA_RESOURCE_DIR ${DATA_RESOURCE_CPP} DIRECTORY)
list(APPEND RESOURCE_DIRS ${DATA_RESOURCE_DIR})
message(DEBUG "Data resource: ${DATA_RESOURCE_CPP}")

set(RESOURCE_GRAMMAR_DATA ${CMAKE_CURRENT_SOURCE_DIR}/org/unicode/inflection/features/grammar.xml)
add_custom_command(
    OUTPUT ${RESOURCE_GRAMMAR_DATA_HPP}
    COMMAND xml2cpp ${RESOURCE_GRAMMAR_DATA} ${RESOURCE_GRAMMAR_DATA_HPP}
    DEPENDS xml2cpp ${RESOURCE_GRAMMAR_DATA}
)
get_filename_component(RESOURCE_GRAMMAR_DATA_DIR ${RESOURCE_GRAMMAR_DATA_HPP} DIRECTORY)
list(APPEND RESOURCE_DIRS ${RESOURCE_GRAMMAR_DATA_DIR})
message(DEBUG "Grammar hpp: ${RESOURCE_GRAMMAR_DATA_HPP}")

set(RESOURCE_CONFIGURATION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/share/inflection/config.properties)
add_custom_command(
    OUTPUT ${RESOURCE_VERSION_H}
    COMMAND echo "\\#pragma once" > ${RESOURCE_VERSION_H}
    COMMAND echo "/* INFLECTION_CAPI */" >> ${RESOURCE_VERSION_H}
    COMMAND sed -n "'s/^library\\.version=\\([0-9]*\\)\\.\\([0-9]*\\)/#define INFLECTION_MAJOR_VERSION \\1/p'" ${RESOURCE_CONFIGURATION_FILE} >> ${RESOURCE_VERSION_H}
    COMMAND sed -n "'s/^library\\.version=\\([0-9]*\\)\\.\\([0-9]*\\)/#define INFLECTION_MINOR_VERSION \\2/p'" ${RESOURCE_CONFIGURATION_FILE} >> ${RESOURCE_VERSION_H}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/share/inflection/config.properties
)

if(XCODE)
    add_library(resource_objs INTERFACE)
    target_sources(resource_objs INTERFACE ${RESOURCES_CPP} ${DATA_RESOURCE_CPP})
    target_include_directories(resource_objs INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
    source_group(TREE ${CMAKE_CURRENT_BINARY_DIR} FILES ${RESOURCES_CPP} ${DATA_RESOURCE_CPP})
else()
    add_library(resource_objs OBJECT)
    target_sources(resource_objs PRIVATE ${RESOURCES_CPP} ${DATA_RESOURCE_CPP})
    set_property(TARGET resource_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(resource_objs
        PRIVATE ${CMAKE_SOURCE_DIR}/src
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_custom_target(generated_src DEPENDS ${RESOURCES_CPP} ${DATA_RESOURCE_CPP} ${RESOURCE_GRAMMAR_DATA_HPP})
add_custom_target(generated_version_header DEPENDS ${RESOURCE_VERSION_H})
add_dependencies(resource_objs generated_src)

# ====== Dictionary files section ==========

set(DICTIONARY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/org/unicode/inflection/dictionary)
set(BINARY_DICTS_DIR ${INFLECTION_DATA_ROOT}/inflection/dictionary)
file(MAKE_DIRECTORY ${BINARY_DICTS_DIR})

file(GLOB_RECURSE BINARY_DICTS_SRC CONFIGURE_DEPENDS ${DICTIONARY_ROOT}/dictionary_*.lst)
file(GLOB_RECURSE INFLECTIONAL_XML_SRC CONFIGURE_DEPENDS ${DICTIONARY_ROOT}/inflectional_*.xml)

list(TRANSFORM BINARY_DICTS_SRC REPLACE ".*/dictionary_(.*)\\.lst" "\\1" OUTPUT_VARIABLE BINARY_DICT_LOCALES)
set(AFFIX_PATTERN_MAPPING_LOCALES fi)

print_debug_list(BINARY_DICT_LOCALES "Locales having dictionary in inflection = ")

set(LIBRARY_PATH_NAME LD_LIBRARY_PATH)
if (APPLE)
    set(LIBRARY_PATH_NAME DYLD_LIBRARY_PATH)
endif()

foreach (LOCALE IN LISTS BINARY_DICT_LOCALES)

    set(BINARY_DICT_SRC "${BINARY_DICTS_SRC}")
    list(FILTER BINARY_DICT_SRC INCLUDE REGEX "dictionary_${LOCALE}\\.lst")
    set(BINARY_INFLECTIONAL_SRC "${INFLECTIONAL_XML_SRC}")
    list(FILTER BINARY_INFLECTIONAL_SRC INCLUDE REGEX "inflectional_${LOCALE}\\.xml")

    set(BINARY_SUPP_SRC          ${DICTIONARY_ROOT}/supplemental_${LOCALE}.lst)
    set(BINARY_SUPP_SRC_ARG      "")
    set(BINARY_DICT              ${BINARY_DICTS_DIR}/mmappable_${LOCALE}.sdict)
    set(AFFIX_PATTERN_MAPPING_ARG "")

    if (NOT EXISTS ${BINARY_SUPP_SRC})
        set(BINARY_SUPP_SRC "")
        set(BINARY_SUPP_SRC_ARG "")
    else ()
        set(BINARY_SUPP_SRC_ARG --supplementalfile ${BINARY_SUPP_SRC})
    endif ()

    list(APPEND BINARY_DICTS ${BINARY_DICT})

    if (BINARY_INFLECTIONAL_SRC)
        set(BINARY_INFLECTIONAL_SRC_ARG --inflectionfile ${BINARY_INFLECTIONAL_SRC})
    else ()
        set(BINARY_INFLECTIONAL_SRC "")
        set(BINARY_INFLECTIONAL_SRC_ARG "")
    endif ()

    list (FIND AFFIX_PATTERN_MAPPING_LOCALES ${LOCALE} FOUND_LOCALE_INDEX)
    if (${FOUND_LOCALE_INDEX} GREATER -1)
        set(AFFIX_PATTERN_MAPPING_ARG "--affix-patterns-mappings")
    endif()

    add_custom_command(
            OUTPUT ${BINARY_DICT}
            COMMAND ${LIBRARY_PATH_NAME}=${ICU_LIB_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/../tools/buildDictionary/buildDictionary --locale ${LOCALE} --outfile ${BINARY_DICT} --infile ${BINARY_DICT_SRC} ${BINARY_SUPP_SRC_ARG} ${BINARY_INFLECTIONAL_SRC_ARG} ${AFFIX_PATTERN_MAPPING_ARG}
            DEPENDS buildDictionary ${BINARY_DICT_SRC} ${BINARY_SUPP_SRC} ${BINARY_INFLECTIONAL_SRC}
    )
endforeach ()
# -------- End Dictionary files section

# ======== Start tok dictionary files section
set(BINARY_TOK_DICT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/org/unicode/inflection/tokenizer)
set(BINARY_TOK_DICT_DIR_ROOT ${INFLECTION_DATA_ROOT}/inflection/tokenizer)
file(MAKE_DIRECTORY ${BINARY_TOK_DICT_DIR_ROOT})

file(GLOB_RECURSE BINARY_TOK_DICT_SRCS CONFIGURE_DEPENDS ${BINARY_TOK_DICT_SRC_DIR}/*/tokenizer.dictionary)
foreach (BINARY_TOK_DICT_SRC IN LISTS BINARY_TOK_DICT_SRCS)
    string(REGEX REPLACE "^${BINARY_TOK_DICT_SRC_DIR}/(.*)\\.dictionary" "${BINARY_TOK_DICT_DIR_ROOT}/\\1.tokd" BINARY_TOK_DICT ${BINARY_TOK_DICT_SRC})
    get_filename_component(BINARY_TOK_DICT_DIR ${BINARY_TOK_DICT} DIRECTORY)
    list(APPEND RESOURCE_DIRS ${BINARY_TOK_DICT_DIR})
    list(APPEND BINARY_TOK_DICTS ${BINARY_TOK_DICT})

    add_custom_command(
        OUTPUT ${BINARY_TOK_DICT}
        COMMAND ${LIBRARY_PATH_NAME}=${ICU_LIB_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/../tools/buildTokDictionary/buildTokDictionary ${BINARY_TOK_DICT_SRC} ${BINARY_TOK_DICT}
        DEPENDS buildTokDictionary ${BINARY_TOK_DICT_SRC}
    )
endforeach()
# -------- End tok dictionary section

file(GLOB_RECURSE RESOURCE_BINARIES ${CMAKE_CURRENT_SOURCE_DIR}/share/*)
install_build_resources(
        RESOURCE_BINARIES
        ${CMAKE_CURRENT_SOURCE_DIR}/share
        ${INFLECTION_DATA_ROOT}
        RESOURCE_BINARIES_DIST
)

add_custom_target(inflection-data ALL DEPENDS ${BINARY_DICTS} ${BINARY_TOK_DICTS} ${RESOURCE_BINARIES_DIST})

#Make directories for all generated resource files
list(REMOVE_DUPLICATES RESOURCE_DIRS)
file(MAKE_DIRECTORY ${RESOURCE_DIRS})