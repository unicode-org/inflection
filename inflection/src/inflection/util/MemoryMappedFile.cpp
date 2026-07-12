/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/util/MemoryMappedFile.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <inflection/util/AutoFileDescriptor.hpp>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

namespace inflection::util {

MemoryMappedFile::MemoryMappedFile(const std::u16string& path)
{
#ifdef _WIN32
    HANDLE hFile = CreateFileW(reinterpret_cast<const wchar_t*>(path.c_str()), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        throw ::inflection::exception::IOException(u"Could not open file " + path + u"; error code " + inflection::util::StringUtils::to_u16string((int32_t)GetLastError()));
    }
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        throw ::inflection::exception::IOException(u"Could not get file size of " + path);
    }
    this->size = static_cast<size_t>(fileSize.QuadPart);
    HANDLE hMapping = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (hMapping == nullptr) {
        CloseHandle(hFile);
        throw ::inflection::exception::IOException(u"Could not create file mapping of " + path);
    }
    char* mappedFile = static_cast<char*>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
    if (mappedFile == nullptr) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        throw inflection::exception::IOException(u"Memory mapping failure of " + path);
    }
    this->data = mappedFile;
    this->fileHandle = hFile;
    this->mappingHandle = hMapping;
    this->owned = true;
#else
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
#endif
}

MemoryMappedFile::MemoryMappedFile(char* data, size_t size) :
    data(data), size(size), owned(false)
{
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (owned && data) {
#ifdef _WIN32
        UnmapViewOfFile(data);
        CloseHandle(mappingHandle);
        CloseHandle(fileHandle);
#else
        munmap(data, size);
#endif
    }
}

} // namespace inflection::util
