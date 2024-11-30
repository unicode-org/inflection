#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
function(morphuntion_debug_vars)
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
            COMMAND cp ${RESOURCE} ${INSTALLED_RESOURCE}
            DEPENDS ${RESOURCE}
        )
    endforeach()
    list(REMOVE_DUPLICATES RESOURCE_DIRS)
    file(MAKE_DIRECTORY ${RESOURCE_DIRS})
    set(${INSTALLED_RESOURCES} ${_tmp_result} PARENT_SCOPE)
endfunction()

macro(get_version_from_makefile QUERY_VAR)
    execute_process(COMMAND
        sh -c "make -s -f ${VERSIONS_MK_PATH} print-${QUERY_VAR}"
        OUTPUT_VARIABLE ${QUERY_VAR}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

macro(get_option_from_makefile QUERY_VAR)
    execute_process(COMMAND
            sh -c "make -s -f ${OPTIONS_MK_PATH} print-${QUERY_VAR}"
            OUTPUT_VARIABLE ${QUERY_VAR}
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

macro(evaluate_command COMMAND_STR)
    execute_process(COMMAND
        sh -c "${COMMAND_STR}"
        OUTPUT_VARIABLE _tmp_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

function(get_morphuntion_version RET_VAL)
    evaluate_command("sh ${CMAKE_SOURCE_DIR}/tools/scripts/version.sh")
    if (NOT _tmp_result)
        set(_tmp_result snapshot)
    endif()
    set(${RET_VAL} ${_tmp_result} PARENT_SCOPE)
endfunction()

function(get_num_processors RET_VAL)
    evaluate_command("(getconf _NPROCESSORS_ONLN || sysctl -n hw.ncpu || nproc --all) 2>/dev/null")
    set(${RET_VAL} ${_tmp_result} PARENT_SCOPE)
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
