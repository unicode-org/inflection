/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/util/StringUtils.hpp>
#include <cstring>

// RAII wrapper
class INFLECTION_INTERNAL_API inflection::util::MemoryMappedFile final
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
        ::std::memcpy(out, readCursorWrapper, sizeof(X));
    }

    // Disallow returning raw pointers to unaligned data
    template <typename X>
    static void readFromCursor(char* readCursorWrapper, X** out) = delete;

public:
    template <typename X>
    void read(X* out, size_t toRead)
    {
        if ((size - offset) < toRead) {
            throw inflection::exception::IOException(u"Input too small. Expected at least "
                    + inflection::util::StringUtils::to_u16string((int32_t) toRead) +
                    u" bytes; Available: " + inflection::util::StringUtils::to_u16string((int32_t) (size - offset)));
        }
        readFromCursor(data + offset, out);
        offset += toRead;
    }

public:
    template <typename X>
    class UnalignedArray {
        char* data;
        size_t length;
    public:
        UnalignedArray(char* d, size_t l) : data(d), length(l) {}
        UnalignedArray() : data(nullptr), length(0) {}
        X operator[](size_t idx) const {
            X val;
            ::std::memcpy(&val, data + idx * sizeof(X), sizeof(X));
            return val;
        }
        size_t size() const { return length; }
        const X* data_ptr() const { return reinterpret_cast<const X*>(data); }
    };

    template <typename X>
    UnalignedArray<X> readArray(size_t length)
    {
        if ((size - offset) < sizeof(X) * length) {
            throw inflection::exception::IOException(u"Input too small for array");
        }
        char* cursor = data + offset;
        offset += sizeof(X) * length;
        return UnalignedArray<X>(cursor, length);
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

