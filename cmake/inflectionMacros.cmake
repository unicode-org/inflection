#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
function(inflection_debug_vars)
    foreach(VAR IN LISTS ARGV)
        message(DEBUG "${VAR}=${${VAR}}")
    endforeach()
endfunction()

function(print_debug_list LIST_NAME MSG)
    string(JOIN ", " LIST_STR ${${LIST_NAME}})
    set(LIST_STR "[${LIST_STR}]")
    message(DEBUG "${MSG}${LIST_STR}")
endfunction()

function(install_build_resources RESOURCES SOURCE_PREFIX DEST_PREFIX INSTALLED_RESOURCES)
    foreach(RESOURCE IN LISTS ${RESOURCES})
        string(REGEX REPLACE "^${SOURCE_PREFIX}" "${DEST_PREFIX}" INSTALLED_RESOURCE ${RESOURCE})
        list(APPEND _tmp_result ${INSTALLED_RESOURCE})
        get_filename_component(INSTALLED_RESOURCE_DIR ${INSTALLED_RESOURCE} DIRECTORY)
        list(APPEND RESOURCE_DIRS ${INSTALLED_RESOURCE_DIR})
        add_custom_command(
            OUTPUT ${INSTALLED_RESOURCE}
            COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE} ${INSTALLED_RESOURCE}
            DEPENDS ${RESOURCE}
        )
    endforeach()
    list(REMOVE_DUPLICATES RESOURCE_DIRS)
    file(MAKE_DIRECTORY ${RESOURCE_DIRS})
    set(${INSTALLED_RESOURCES} ${_tmp_result} PARENT_SCOPE)
endfunction()

function(read_mk_variable MK_FILE QUERY_VAR)
    file(STRINGS ${MK_FILE} _lines REGEX "^${QUERY_VAR}[ \t]*[:?]?=")
    if (_lines)
        list(GET _lines 0 _line)
        string(REGEX REPLACE "^${QUERY_VAR}[ \t]*[:?]?=[ \t]*" "" _value "${_line}")
        string(STRIP "${_value}" _value)
        set(${QUERY_VAR} "${_value}" PARENT_SCOPE)
    endif()
endfunction()

macro(get_version_from_makefile QUERY_VAR)
    read_mk_variable(${CMAKE_SOURCE_DIR}/cmake/versions.mk ${QUERY_VAR})
endmacro()

macro(get_option_from_makefile QUERY_VAR)
    if (EXISTS ${CMAKE_SOURCE_DIR}/options.mk)
        read_mk_variable(${CMAKE_SOURCE_DIR}/options.mk ${QUERY_VAR})
    endif()
endmacro()

function(get_inflection_version RET_VAL)
    execute_process(
        COMMAND git tag --points-at HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _tags
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    set(_best_version "")
    if (_tags)
        string(REPLACE "\n" ";" _tag_list "${_tags}")
        foreach(_tag IN LISTS _tag_list)
            if (_tag MATCHES "^Inflection-([0-9.]+)$")
                set(_ver "${CMAKE_MATCH_1}")
                if ("${_best_version}" STREQUAL "" OR "${_ver}" VERSION_GREATER "${_best_version}")
                    set(_best_version "${_ver}")
                endif()
            endif()
        endforeach()
    endif()
    if (NOT _best_version)
        set(_best_version snapshot)
    endif()
    set(${RET_VAL} ${_best_version} PARENT_SCOPE)
endfunction()

function(get_num_processors RET_VAL)
    cmake_host_system_information(RESULT _num_procs QUERY NUMBER_OF_LOGICAL_CORES)
    set(${RET_VAL} ${_num_procs} PARENT_SCOPE)
endfunction()

function(fixRuntimePath TARGET DEPLIB RELATIVE_RUNTIMEPATH)
    set(LIBFILE $<TARGET_FILE_NAME:${DEPLIB}>)
    if(APPLE)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND install_name_tool -change $<TARGET_PROPERTY:${DEPLIB},INSTALL_NAME_DIR>/${LIBFILE} "@loader_path/${RELATIVE_RUNTIMEPATH}/${LIBFILE}" $<TARGET_FILE:${TARGET}>
            COMMENT "Fix library runtime path for ${TARGET} binary"
            VERBATIM
        )
        set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH @loader_path/${RELATIVE_RUNTIMEPATH})
    elseif(UNIX)
        set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH $ORIGIN/${RELATIVE_RUNTIMEPATH})
    endif ()
endfunction()
