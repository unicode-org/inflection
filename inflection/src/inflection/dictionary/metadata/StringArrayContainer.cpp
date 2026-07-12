/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/metadata/StringArrayContainer.hpp>

#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/npc.hpp>
#include <numeric>
#include <string.h>

static constexpr int32_t LENGTH_BITS = 8;
static constexpr int32_t LENGTH_MASK = (1 << LENGTH_BITS) - 1;

inflection::dictionary::metadata::StringArrayContainer::StringArrayContainer(::inflection::util::MemoryMappedFile *mappedFile)
    : arraySize(npc(mappedFile)->read<int32_t>())
    , allStringsSize(npc(mappedFile)->read<int32_t>())
    , stringIndexesWithLen(npc(mappedFile)->readArray<int32_t>(arraySize))
    , allStrings(npc(mappedFile)->readArray<char16_t>(allStringsSize))
{
}

inflection::dictionary::metadata::StringArrayContainer::StringArrayContainer(const ::std::set<::std::u16string_view>& strings, ::std::map<::std::u16string_view, int32_t>* mappingResult)
    : arraySize(int32_t(strings.size()))
    , stringIndexesWithLen(new int32_t[arraySize])
{
    if (!npc(mappingResult)->empty()) {
        throw ::inflection::exception::IllegalArgumentException(u"The mapping result should be empty");
    }

    // concatenate all strings together, but reuse duplicate strings
    ::std::u16string allStringsBuffer;
    allStringsBuffer.reserve(strings.size());
    int32_t stringIndexesIdx = 0;

    for (auto string : strings) {
        int32_t offset = int32_t(allStringsBuffer.length());
        int32_t len = int32_t(string.length());
        if (len > LENGTH_MASK || ((offset << LENGTH_BITS) >> LENGTH_BITS) != offset) {
            throw ::inflection::exception::IllegalArgumentException(u"String is too large: " + std::u16string(string));
        }
        stringIndexesWithLen[stringIndexesIdx] = (offset << LENGTH_BITS) | len;
        allStringsBuffer.append(string);
        allStringsBuffer.push_back(0); // null terminate
        npc(mappingResult)->emplace(string, stringIndexesIdx);
        stringIndexesIdx++;
    }

    // Pad to align other data structures to int64_t alignment.
    int32_t paddingSize = (sizeof(int64_t) - (sizeof(arraySize) + sizeof(allStringsSize) + (arraySize * sizeof(stringIndexesWithLen[0])) + (allStringsBuffer.length() * sizeof(allStringsBuffer[0]))) % sizeof(int64_t)) / sizeof(char16_t);
    allStringsBuffer.append(paddingSize, 0);

    allStringsSize = (int32_t)allStringsBuffer.length();
    allStrings = new char16_t[allStringsSize];
    memcpy(allStrings, allStringsBuffer.data(), allStringsSize * sizeof(allStrings[0]));
    ownData = true;
}

inflection::dictionary::metadata::StringArrayContainer::StringArrayContainer()
    : ownData(false)
{
}

inflection::dictionary::metadata::StringArrayContainer::~StringArrayContainer()
{
    if (ownData) {
        delete[] stringIndexesWithLen;
        delete[] allStrings;
    }
}

void inflection::dictionary::metadata::StringArrayContainer::write(::std::ostream& output) const
{
    output.write(reinterpret_cast<const char*>(&arraySize), sizeof(arraySize));
    output.write(reinterpret_cast<const char*>(&allStringsSize), sizeof(allStringsSize));

    output.write(reinterpret_cast<const char*>(stringIndexesWithLen), arraySize * sizeof(stringIndexesWithLen[0]));
    output.write(reinterpret_cast<const char*>(allStrings), allStringsSize * sizeof(allStrings[0]));
}

::std::u16string inflection::dictionary::metadata::StringArrayContainer::getString(int32_t offset) const
{
    if (offset < 0 || arraySize <= offset) {
        throw ::inflection::exception::IndexOutOfBoundsException(u"Offset is out of bounds");
    }
    int32_t value = stringIndexesWithLen[offset];
    int32_t len = value & LENGTH_MASK;
    int32_t stringStart = value >> LENGTH_BITS;
    return std::u16string(allStrings + stringStart, len);
}

int32_t inflection::dictionary::metadata::StringArrayContainer::getIdentifierIfAvailable(::std::u16string_view string) const
{
    int32_t start = 0;
    int32_t limit = arraySize - 1;
    while (start <= limit) {
        int32_t mid = std::midpoint(start, limit);

        int32_t value = stringIndexesWithLen[mid];
        int32_t len = value & LENGTH_MASK;
        int32_t stringStart = uint32_t(value) >> LENGTH_BITS;

        std::u16string_view strToCompare(allStrings + stringStart, len);

        auto comp = string.compare(strToCompare);
        if (comp < 0) {
            limit = mid - 1;
        }
        else if (comp > 0) {
            start = mid + 1;
        }
        else {
            return mid;
        }
    }

    return -1;
}

int32_t inflection::dictionary::metadata::StringArrayContainer::getIdentifier(::std::u16string_view string) const
{
    auto result = getIdentifierIfAvailable(string);
    if (result < 0) {
        throw ::inflection::exception::IllegalArgumentException(::std::u16string(u"String not found: ") + ::std::u16string(string));
    }
    return result;
}

int32_t inflection::dictionary::metadata::StringArrayContainer::size() const {
    return arraySize;
}
