/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#include "DictionaryLogger.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>


DictionaryLogger::DictionaryLogger(std::ofstream& writer, bool verbose)
    : prevOffset(writer.tellp())
    , writer(writer)
    , verbose(verbose)
{
}

static constexpr auto KILOBYTE = 1024;

void DictionaryLogger::log(std::string_view message) const
{
    if (verbose) {
        std::cout << message << ::std::endl;
    }
}

void DictionaryLogger::logWithOffset(std::string_view message)
{
    if (verbose) {
        size_t newOffset = writer.tellp();
        std::ios_base::fmtflags originalFlags(std::cout.flags());
        std::cout << message << ": offset=" << std::uppercase << std::showbase << std::hex << uintptr_t(prevOffset);
        std::cout.flags(originalFlags);
        auto size = newOffset - prevOffset;
        std::cout << " size=" << (size) << " bytes (" << ((size) / KILOBYTE) << " KB" << ")" << ::std::endl;
        prevOffset = newOffset;
    }
}

bool DictionaryLogger::isVerbose() const
{
    return verbose;
}