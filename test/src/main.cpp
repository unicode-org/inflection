/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#define CATCH_CONFIG_RUNNER

#include <inflection/util/LoggerConfig.h>

#include "catch2/catch_config.hpp"
#include "catch2/catch_session.hpp"

#include <util/TestUtils.hpp>

#include <unicode/uclean.h>

#include <iostream>

using namespace Catch::Clara;

int main(int argc, const char* const argv[])
{
    auto [session, result, originalLogLevel] = TestUtils::createSession(argc, argv);

    // Something bad happened while creating the session
    if (result != 0)
    {
        std::cerr << "Catch2 session creation failed with error code (read above for why): " << result << std::endl;
        return result;
    }

    // Run catch2
    result = session->run();

    // Cleanup ICU to make it easier to debug valgrind issues.
    u_cleanup();
    TestUtils::cleanup();

    if (originalLogLevel != ilogc_getLogLevel()) {
        std::cerr << "The log level changed during execution, and it should remain unchanged during execution of the tests." << ::std::endl;
        result++;
    }

    return result;
}
