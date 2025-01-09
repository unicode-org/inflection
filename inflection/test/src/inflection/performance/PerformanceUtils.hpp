/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/ULocale.hpp>

#include <vector>
#include <string>
#include <functional>

size_t PerformanceUtils_getPeakRSS();
size_t PerformanceUtils_getCurrentRSS();

template<typename RecordObjectType>
class PerfTable
{
public:
    PerfTable(const std::string& tableName);
    ~PerfTable();
    
    void writeRow(std::function<void(RecordObjectType&)> write_function);
    
private:
    RecordObjectType recordObject;
    std::string filepath;
    std::string tableName;
};
