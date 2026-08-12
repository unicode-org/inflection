/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <string>

// Wrapper around a file descriptor that close()s on when dtor'd if valid
class inflection::util::AutoFileDescriptor {
public:
    // Wraps open()
    AutoFileDescriptor(const char *path, int flags);

    ~AutoFileDescriptor();

    int getFD() const {
        return fd;
    }

    static bool isAccessibleFile(std::u16string_view path);

private:
    // Not copyable
    AutoFileDescriptor(const AutoFileDescriptor& other) = delete;
    AutoFileDescriptor& operator=(const AutoFileDescriptor& other) = delete;

    int fd;
};
