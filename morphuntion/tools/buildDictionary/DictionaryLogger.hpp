/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <ostream>
#include <string_view>

class DictionaryLogger final
{
private:
    size_t prevOffset;
    std::ofstream& writer;
    bool verbose;

public:
    void log(std::string_view message) const;
    void logWithOffset(std::string_view message);
    bool isVerbose() const;
    DictionaryLogger(std::ofstream& writer, bool verbose);
};
