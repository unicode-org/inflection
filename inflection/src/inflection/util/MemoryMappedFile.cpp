/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/AutoFileDescriptor.hpp>
#include <inflection/util/MemoryMappedFile.hpp>

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

namespace inflection::util {

MemoryMappedFile::MemoryMappedFile(const std::u16string& path)
{
    ::std::string cPath;
    StringUtils::convert(&cPath, path);
    AutoFileDescriptor file(cPath.c_str(), O_RDONLY);
    if (file.getFD() < 0) {
        throw ::inflection::exception::IOException(u"Could not open file " + path);
    }
    struct stat fileStat {};
    if (fstat(file.getFD(), &fileStat) < 0 || !S_ISREG(fileStat.st_mode)) {
        throw ::inflection::exception::IOException(path + u" is not a regular file");
    }
    this->size = fileStat.st_size;
    char* mappedFile = (char*) mmap(nullptr, size, PROT_READ, MAP_PRIVATE, file.getFD(), 0);
    if (mappedFile == MAP_FAILED) {
        throw inflection::exception::IOException(u"Memory mapping failure of " + path);
    }
    this->data = mappedFile;
    this->owned = true;
}

MemoryMappedFile::MemoryMappedFile(char* data, size_t size) :
    data(data), size(size), owned(false)
{
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (owned && data) {
        munmap(data, size);
    }
}

} // namespace inflection::util
