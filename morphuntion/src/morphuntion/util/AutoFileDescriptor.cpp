/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/AutoFileDescriptor.hpp>

#include <morphuntion/util/StringViewUtils.hpp>
#include <fcntl.h>
#include <unistd.h>

namespace morphuntion::util {


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
    morphuntion::util::StringViewUtils::convert(&cPath, path);
    return access(cPath.c_str(), F_OK) == 0;
}

} // namespace morphuntion::util
