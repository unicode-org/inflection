/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/exception/IOException.hpp>
#include <morphuntion/util/StringUtils.hpp>

// RAII wrapper
class MORPHUNTION_INTERNAL_API morphuntion::util::MemoryMappedFile final
{
public:
    explicit MemoryMappedFile(const std::u16string& path);

    MemoryMappedFile(char* data, size_t size);

    ~MemoryMappedFile();

    size_t getSize() const {
        return size;
    }

    const char* getData() const {
        return data;
    }

private:
    template <typename X>
    static void readFromCursor(char* readCursorWrapper, X* out)
    {
        *out = *((X*) readCursorWrapper);
    }

    template <typename X>
    static void readFromCursor(char* readCursorWrapper, X** out)
    {
        *out = (X*) readCursorWrapper;
    }

public:
    template <typename X>
    void read(X* out, size_t toRead)
    {
        if ((size - offset) < toRead) {
            throw morphuntion::exception::IOException(u"Input too small. Expected at least "
                    + morphuntion::util::StringUtils::to_u16string((int32_t) toRead) +
                    u" bytes; Available: " + morphuntion::util::StringUtils::to_u16string((int32_t) (size - offset)));
        }
        readFromCursor(data + offset, out);
        offset += toRead;
    }

    template <typename X>
    X* readArray(size_t length)
    {
        X* out = nullptr;
        read(&out, sizeof(*out)*length);
        return out;
    }

    template <typename X>
    void read(X* out)
    {
        read(out, sizeof(*out));
    }

    template <typename X>
    X read()
    {
        X out;
        read(&out, sizeof(out));
        return out;
    }

private:
    MemoryMappedFile(MemoryMappedFile&& other) = delete;
    MemoryMappedFile(const MemoryMappedFile& other) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile& other) = delete;

    char* data = {  };
    size_t size = {  };
    size_t offset = {  };
    bool owned = {  };
};

