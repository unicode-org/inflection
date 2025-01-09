/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
//
// Created by Nolan Kramer on 2021-05-04.
//

#include "PerformanceUtils.hpp"

#include <inflection/util/StringUtils.hpp>
#include <iostream>
#include <fstream>
#include <limits>

// include headers necessary for measuring heap perf
#include <unistd.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>

#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."

#endif

template<>
PerfTable<std::ofstream>::PerfTable(const std::string& tableName)
: tableName(tableName)
{
    this->filepath = "/tmp/" + tableName;
    this->recordObject.open(this->filepath);
}

template<>
void PerfTable<std::ofstream>::writeRow(std::function<void(std::ofstream&)> write_function)
{
    write_function(this->recordObject);
}

template<>
PerfTable<std::ofstream>::~PerfTable()
{
    this->recordObject.close();
    std::cout << "Results written to " << this->filepath << std::endl;
}

// See http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use for full implementations (including Windows)
/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t PerformanceUtils_getPeakRSS()
{
    /* BSD, Linux, and OSX -------------------------------------- */
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
#if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t PerformanceUtils_getCurrentRSS()
{
#if defined(__APPLE__) && defined(__MACH__)
    /* OSX ------------------------------------------------------ */
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info( mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS)
        return (size_t)0L;      /* Can't access? */
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    /* Linux ---------------------------------------------------- */
    long rss = 0L;
    FILE* fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r" )) == NULL)
        return (size_t)0L;      /* Can't open? */
    if (fscanf(fp, "%*s%ld", &rss) != 1)
    {
        fclose(fp);
        return (size_t)0L;      /* Can't read? */
    }
    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);

#else
    /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
    return (size_t)0L;          /* Unsupported. */
#endif
}
