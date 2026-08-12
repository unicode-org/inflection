#
# Copyright 2021-2026 Apple Inc. All rights reserved.
#
set(LIBRARY_PATH_NAME LD_LIBRARY_PATH)
if (APPLE)
    set(LIBRARY_PATH_NAME DYLD_LIBRARY_PATH)
endif()

if(WIN32)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "Test command: ${EXECUTABLE} ${TEST_OPTS}"
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${DYLD_LIBRARY_PATH};$ENV{PATH}" ${EXECUTABLE} ${TEST_OPTS}
        RESULT_VARIABLE RETURN_CODE
    )
else()
    set(RUN_COMMAND "${LIBRARY_PATH_NAME}=\${${LIBRARY_PATH_NAME}}:${DYLD_LIBRARY_PATH} $TEST_RUN_COMMAND ${EXECUTABLE} $TEST_OPTS")
    execute_process(
            COMMAND sh -c "echo Test command: ${RUN_COMMAND}"
    )
    execute_process(
            COMMAND sh -c "${RUN_COMMAND}"
            OUTPUT_FILE /dev/stdout
            ERROR_FILE /dev/stderr
            RESULT_VARIABLE RETURN_CODE
    )
endif()

if(RETURN_CODE)
    message(FATAL_ERROR "make check failed")
endif()
