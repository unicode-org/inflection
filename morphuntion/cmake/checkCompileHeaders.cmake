#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
string(CONCAT HEADER_INCLUDES "-I" ${HEADER_DIR})
if(NOT ${CF_INCLUDES} STREQUAL "")
    string(APPEND HEADER_INCLUDES ";" ${CF_INCLUDES})
endif()
if(NOT ${ICU_INCLUDES} STREQUAL "")
    string(APPEND HEADER_INCLUDES ";" ${ICU_INCLUDES})
endif()

file(GLOB_RECURSE MORPHUNTION_CPP_HEADERS "${HEADER_DIR}/*.h" "${HEADER_DIR}/*.hpp")
file(GLOB_RECURSE MORPHUNTION_C_HEADERS "${HEADER_DIR}/*.h")

function(compile_headers COMPILER_BIN COMPILER_FLAGS COMPILER_INCLUDES SOURCE_TYPE HEADERS)
    foreach(HEADER IN LISTS HEADERS)
        string(REGEX REPLACE "^${HEADER_DIR}/" "" HEADER_INCLUDE_STRING ${HEADER})

        execute_process(
            COMMAND echo "#include <${HEADER_INCLUDE_STRING}>"
            COMMAND ${COMPILER_BIN} -pipe ${COMPILER_FLAGS} ${COMPILER_INCLUDES} -x${SOURCE_TYPE} - -c -o /dev/null
            OUTPUT_FILE /dev/stdout
            COMMAND_ECHO STDOUT
            RESULT_VARIABLE RETURN_CODE
        )

        if(RETURN_CODE)
            message(FATAL_ERROR "make check-headers failed")
        endif()
    endforeach()
endfunction()

compile_headers(${CPP_COMPILER_BIN} "${CXX_CHECK_HEADER_FLAGS};-std=c++${CXX_STANDARD}" "${HEADER_INCLUDES}" "c++" "${MORPHUNTION_CPP_HEADERS}")
compile_headers(${C_COMPILER_BIN} "-std=c11" "${HEADER_INCLUDES}" "c" "${MORPHUNTION_C_HEADERS}")
