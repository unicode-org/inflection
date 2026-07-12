#!/bin/sh
#
# Copyright 2026 Apple Inc. All rights reserved.
#
# Build and run tests with parallel compilation.
# Usage: tools/scripts/check.sh [build-dir] [TEST_OPTS...]
#
# Examples:
#   tools/scripts/check.sh
#   tools/scripts/check.sh build
#   tools/scripts/check.sh build-claude "-l nl InflectionTest#testInflections"

BUILD_DIR="${1:-build}"
shift 2>/dev/null
TEST_OPTS="$*"

JOBS="$(getconf _NPROCESSORS_ONLN)"

if [ -n "$TEST_OPTS" ]; then
    cmake --build "$BUILD_DIR" -j "$JOBS" --target check -- TEST_OPTS="$TEST_OPTS"
else
    cmake --build "$BUILD_DIR" -j "$JOBS" --target check
fi
