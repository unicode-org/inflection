#
# Copyright 2021-2026 Apple Inc. All rights reserved.
#
set(RESOURCE_GRAMMAR_DATA_HPP ${CMAKE_BINARY_DIR}/resources/inflection/lang/features/LanguageGrammarFeatures_Data.hpp)
set(RESOURCE_VERSION_H ${CMAKE_BINARY_DIR}/resources/inflection/version.h)

file(GLOB_RECURSE RESOURCE_PROPS ${CMAKE_CURRENT_SOURCE_DIR}/org/[^.]*.properties)
file(GLOB_RECURSE RESOURCE_DICTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/org/[^.]*)
foreach (FILTER_REGEX IN ITEMS "\\.(properties|dtd|xml|lst)$" "tokenizer\\.dictionary$" "transliteration_map_.+\\.csv$" "suffix_.+\\.csv$")
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
set(DATA_RESOURCE_GEN ${CMAKE_SOURCE_DIR}/cmake/GenerateDataResource.cmake)
add_custom_command(
    OUTPUT ${DATA_RESOURCE_CPP}
    COMMAND ${CMAKE_COMMAND} -DDATA_RESOURCE_PATH="${DATA_RESOURCE_CPP}" -DRESOURCES="${CMAKE_SOURCE_DIR}/resources" -P  "${DATA_RESOURCE_GEN}"
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
    COMMAND ${CMAKE_COMMAND}
        -D "CONFIG_FILE=${RESOURCE_CONFIGURATION_FILE}"
        -D "OUTPUT_FILE=${RESOURCE_VERSION_H}"
        -P ${CMAKE_SOURCE_DIR}/cmake/GenerateVersionHeader.cmake
    DEPENDS ${RESOURCE_CONFIGURATION_FILE}
)

add_library(resource_objs OBJECT)
target_sources(resource_objs PRIVATE ${RESOURCES_CPP} ${DATA_RESOURCE_CPP})
set_property(TARGET resource_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(resource_objs
    PRIVATE ${CMAKE_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

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

print_debug_list(BINARY_DICT_LOCALES "Locales having dictionary in inflection = ")

if (APPLE)
    set(LIBRARY_PATH_NAME DYLD_LIBRARY_PATH)
elseif (WIN32)
    set(LIBRARY_PATH_NAME PATH)
else()
    set(LIBRARY_PATH_NAME LD_LIBRARY_PATH)
endif()

foreach (LOCALE IN LISTS BINARY_DICT_LOCALES)

    set(BINARY_DICT_SRC "${BINARY_DICTS_SRC}")
    list(FILTER BINARY_DICT_SRC INCLUDE REGEX "dictionary_${LOCALE}\\.lst")
    set(BINARY_INFLECTIONAL_SRC "${INFLECTIONAL_XML_SRC}")
    list(FILTER BINARY_INFLECTIONAL_SRC INCLUDE REGEX "inflectional_${LOCALE}\\.xml")

    set(BINARY_DICT              ${BINARY_DICTS_DIR}/mmappable_${LOCALE}.sdict)
    list(APPEND BINARY_DICTS ${BINARY_DICT})

    set(BINARY_SUPP_SRC ${DICTIONARY_ROOT}/supplemental_${LOCALE}.lst)
    if (EXISTS ${BINARY_SUPP_SRC})
        set(BINARY_SUPP_SRC_ARG --supplementalfile ${BINARY_SUPP_SRC})
    else ()
        set(BINARY_SUPP_SRC "")
        set(BINARY_SUPP_SRC_ARG "")
    endif ()

    if (BINARY_INFLECTIONAL_SRC)
        set(BINARY_INFLECTIONAL_SRC_ARG --inflectionfile ${BINARY_INFLECTIONAL_SRC})
    else ()
        set(BINARY_INFLECTIONAL_SRC "")
        set(BINARY_INFLECTIONAL_SRC_ARG "")
    endif ()

    add_custom_command(
            OUTPUT ${BINARY_DICT}
            COMMAND ${CMAKE_COMMAND} -E env "${LIBRARY_PATH_NAME}=${ICU_LIB_DIRECTORY}" $<TARGET_FILE:buildDictionary> --locale ${LOCALE} --outfile ${BINARY_DICT} --infile ${BINARY_DICT_SRC} ${BINARY_SUPP_SRC_ARG} ${BINARY_INFLECTIONAL_SRC_ARG}
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
        COMMAND ${CMAKE_COMMAND} -E env "${LIBRARY_PATH_NAME}=${ICU_LIB_DIRECTORY}" $<TARGET_FILE:buildTokDictionary> ${BINARY_TOK_DICT_SRC} ${BINARY_TOK_DICT}
        DEPENDS buildTokDictionary ${BINARY_TOK_DICT_SRC}
    )
endforeach()
# -------- End tok dictionary section

# -------- Begin suffix exemplar section
set(SUFFIX_EXEMPLAR_MAP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/org/unicode/inflection/exemplar)
set(SUFFIX_EXEMPLAR_MAP_DIR_ROOT ${INFLECTION_DATA_ROOT}/inflection/exemplar)
file(GLOB_RECURSE SUFFIX_EXEMPLAR_CSVS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/org/unicode/inflection/exemplar/*.csv)
file(MAKE_DIRECTORY ${SUFFIX_EXEMPLAR_MAP_DIR_ROOT})
foreach (SUFFIX_EXEMPLAR_CSV IN LISTS SUFFIX_EXEMPLAR_CSVS)
    string(REGEX REPLACE "^${SUFFIX_EXEMPLAR_MAP_SRC_DIR}/(.*)\\.csv" "${SUFFIX_EXEMPLAR_MAP_DIR_ROOT}/\\1.bist" SUFFIX_EXEMPLAR_BIST ${SUFFIX_EXEMPLAR_CSV})
    get_filename_component(SUFFIX_EXEMPLAR_MAP_DIR ${SUFFIX_EXEMPLAR_BIST} DIRECTORY)
    list(APPEND RESOURCE_DIRS ${SUFFIX_EXEMPLAR_MAP_DIR})
    list(APPEND SUFFIX_EXEMPLAR_MAPS ${SUFFIX_EXEMPLAR_BIST})

    add_custom_command(
            OUTPUT ${SUFFIX_EXEMPLAR_BIST}
            COMMAND ${CMAKE_COMMAND} -E env "${LIBRARY_PATH_NAME}=${ICU_LIB_DIRECTORY}" $<TARGET_FILE:buildStringMap> ${SUFFIX_EXEMPLAR_CSV} ${SUFFIX_EXEMPLAR_BIST}
            DEPENDS buildStringMap ${SUFFIX_EXEMPLAR_CSV}
    )
endforeach()

# -------- End suffix exemplar section

file(GLOB_RECURSE RESOURCE_BINARIES ${CMAKE_CURRENT_SOURCE_DIR}/share/*)
install_build_resources(
        RESOURCE_BINARIES
        ${CMAKE_CURRENT_SOURCE_DIR}/share
        ${INFLECTION_DATA_ROOT}
        RESOURCE_BINARIES_DIST
)

add_custom_target(inflection-data ALL DEPENDS ${BINARY_DICTS} ${BINARY_TOK_DICTS} ${SUFFIX_EXEMPLAR_MAPS} ${RESOURCE_BINARIES_DIST})

#Make directories for all generated resource files
list(REMOVE_DUPLICATES RESOURCE_DIRS)
file(MAKE_DIRECTORY ${RESOURCE_DIRS})