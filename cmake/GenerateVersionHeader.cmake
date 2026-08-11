#
# Copyright 2026 Unicode Incorporated and others. All rights reserved.
#
# Invoked with: cmake -D CONFIG_FILE=... -D OUTPUT_FILE=... -P GenerateVersionHeader.cmake
file(STRINGS "${CONFIG_FILE}" _version_line REGEX "^library\\.version=")
string(REGEX MATCH "^library\\.version=([0-9]+)\\.([0-9]+)" _ "${_version_line}")
file(WRITE "${OUTPUT_FILE}"
    "#pragma once\n/* INFLECTION_CAPI */\n#define INFLECTION_MAJOR_VERSION ${CMAKE_MATCH_1}\n#define INFLECTION_MINOR_VERSION ${CMAKE_MATCH_2}\n")
