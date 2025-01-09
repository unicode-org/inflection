#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
set(LIBRARY_PATH_NAME LD_LIBRARY_PATH)
if (APPLE)
    set(LIBRARY_PATH_NAME DYLD_LIBRARY_PATH)
endif()
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

if(RETURN_CODE)
    message(FATAL_ERROR "make check failed")
endif()
