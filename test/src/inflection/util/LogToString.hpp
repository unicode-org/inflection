/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <string>
#include <vector>

class LogToString {
public:
    std::vector<std::u16string> logLines;
    LogToString();
    ~LogToString();
};