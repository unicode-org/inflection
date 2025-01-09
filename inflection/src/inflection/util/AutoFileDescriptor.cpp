/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/AutoFileDescriptor.hpp>

#include <inflection/util/StringViewUtils.hpp>
#include <fcntl.h>
#include <unistd.h>

namespace inflection::util {


AutoFileDescriptor::AutoFileDescriptor(const char *path, int f)
    : fd(::open(path, f))
{
}

AutoFileDescriptor::~AutoFileDescriptor()
{
    if (fd >= 0) {
        ::close(fd);
    }
}

bool AutoFileDescriptor::isAccessibleFile(std::u16string_view path)
{
    ::std::string cPath;
    inflection::util::StringViewUtils::convert(&cPath, path);
    return access(cPath.c_str(), F_OK) == 0;
}

} // namespace inflection::util
