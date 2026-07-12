#!/bin/sh
#
# Copyright 2026 Unicode Incorporated and others. All rights reserved.
#
# Generate an HTML code coverage report for src/inflection using LLVM
# source-based coverage (enabled by the PROFILING CMake option).
#
# Usage: tools/scripts/coverage.sh [build-dir]
#   build-dir defaults to "build-coverage", kept separate from the normal
#   build dir so instrumentation flags never leak into regular builds.
#
set -e

BUILD_DIR="${1:-build-coverage}"

# Resolve the inflection source dir relative to this script's own location,
# so the script works regardless of the current directory.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOURCE_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"   # tools/scripts -> inflection

JOBS="$(getconf _NPROCESSORS_ONLN)"

# Resolve an LLVM coverage tool. Coverage uses LLVM source-based coverage; the
# PROFILING flags (-fprofile-instr-generate -fcoverage-mapping) are Clang-only.
# Prefer xcrun on macOS, fall back to the bare name, then versioned variants.
resolve_tool() {
    # $1 = base tool name (llvm-profdata / llvm-cov)
    if command -v xcrun >/dev/null 2>&1 && xcrun --find "$1" >/dev/null 2>&1; then
        echo "xcrun $1"
        return 0
    fi
    if command -v "$1" >/dev/null 2>&1; then
        echo "$1"
        return 0
    fi
    for v in 21 20 19 18 17 16 15 14; do
        if command -v "$1-$v" >/dev/null 2>&1; then
            echo "$1-$v"
            return 0
        fi
    done
    return 1
}

LLVM_PROFDATA="$(resolve_tool llvm-profdata)" || {
    echo "error: llvm-profdata not found; coverage requires the LLVM toolchain" >&2
    exit 1
}
LLVM_COV="$(resolve_tool llvm-cov)" || {
    echo "error: llvm-cov not found; coverage requires the LLVM toolchain" >&2
    exit 1
}

# --- Configure + build the instrumented test binary -------------------------
cmake -B "$BUILD_DIR" -S "$SOURCE_DIR" -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" -j "$JOBS" --target itest

# --- Run the instrumented tests ---------------------------------------------
COV_DIR="$(cd "$BUILD_DIR" && pwd)/coverage"
mkdir -p "$COV_DIR"
rm -f "$COV_DIR"/*.profraw

# LLVM_PROFILE_FILE is read by the instrumented binary at runtime. Exporting it
# here propagates through the `check` target down to the itest run, which also
# reuses runTests.cmake's library-path setup (libinflection + ICU). %p keeps the
# raw profile per-process.
export LLVM_PROFILE_FILE="$COV_DIR/inflection-%p.profraw"

TEST_FAILED=0
cmake --build "$BUILD_DIR" -j "$JOBS" --target check || TEST_FAILED=1

# --- Merge raw profiles ------------------------------------------------------
if ! ls "$COV_DIR"/*.profraw >/dev/null 2>&1; then
    echo "error: no .profraw files produced; PROFILING instrumentation did not take effect" >&2
    exit 1
fi

PROFDATA="$COV_DIR/inflection.profdata"
$LLVM_PROFDATA merge -sparse "$COV_DIR"/*.profraw -o "$PROFDATA"

# --- Locate the instrumented library and render the HTML report -------------
LIB="$(find "$BUILD_DIR" \( -name 'libinflection.dylib' -o -name 'libinflection.so' \) -type f | head -n1)"
if [ -z "$LIB" ]; then
    echo "error: could not locate libinflection in $BUILD_DIR" >&2
    exit 1
fi

HTML_DIR="$COV_DIR/html"
# llvm-cov matches -ignore-filename-regex against the absolute source paths
# embedded in the binary; the unanchored substrings below match as intended.
$LLVM_COV show "$LIB" \
    -instr-profile="$PROFDATA" \
    -format=html \
    -output-dir="$HTML_DIR" \
    -ignore-filename-regex='(ext/.*|src/icu4cxx/.*|test/.*)'

if [ "$TEST_FAILED" -ne 0 ]; then
    echo "warning: some tests failed; coverage reflects a failed test run" >&2
fi
echo "Coverage report: $HTML_DIR/index.html"
