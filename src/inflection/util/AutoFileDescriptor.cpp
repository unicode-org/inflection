/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/util/AutoFileDescriptor.hpp>

#include <inflection/util/StringViewUtils.hpp>
#include <fcntl.h>
#include <filesystem>
#ifdef _WIN32
#include <io.h>
#define file_open ::_open
#define file_close ::_close
#else
#include <unistd.h>
#define file_open ::open
#define file_close ::close
#endif

namespace inflection::util {

AutoFileDescriptor::AutoFileDescriptor(const char *path, int f)
    : fd(file_open(path, f))
{
}

AutoFileDescriptor::~AutoFileDescriptor()
{
    if (fd >= 0) {
        file_close(fd);
    }
}

bool AutoFileDescriptor::isAccessibleFile(std::u16string_view path)
{
    return std::filesystem::exists(path);
}

} // namespace inflection::util
