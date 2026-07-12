/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/npc.hpp>
#include <cstring>
#include <memory>
#include <sstream>
#include <iosfwd>
#include <vector>

template <typename T>
class inflection::dictionary::metadata::CompressedArray {
public:
    static constexpr int32_t DATAWIDTH = 64;
    static int8_t calculateBitWidth(uint64_t allUsedBits);
    static int8_t calculateBitWidth(const ::std::vector<T>& input);
    static T extractValue(uint64_t value, uint8_t start, uint8_t len);

    bool isEmpty() const;

    T read(int32_t index) const;
    void write(int32_t index, T value);

    void serialize(std::ostream &writer) const;

    explicit CompressedArray(::inflection::util::MemoryMappedFile* mappedFile);
    CompressedArray(int32_t wordWidth, int32_t arraySize);
    explicit CompressedArray(const ::std::vector<T>& input);
    ~CompressedArray();

private:
    int32_t dataArrayLength {  };
    int32_t wordWidth {  };
    uint64_t* data {  };
    uint64_t selectMask {  };
    bool ownData {  };

    CompressedArray(const CompressedArray& other) = delete;
    CompressedArray& operator=(const CompressedArray& other) = delete;
};

template<typename T>
inflection::dictionary::metadata::CompressedArray<T>::CompressedArray(::inflection::util::MemoryMappedFile* mappedFile)
    : ownData(false)
{
    npc(mappedFile)->read(&dataArrayLength);
    mappedFile->read(&wordWidth);
    mappedFile->read(&data, sizeof(data[0]) * dataArrayLength);
    selectMask = ((uint64_t(1) << wordWidth) - 1);
}

// Simple way of taking the ceiling function, keySet.size() * wordWidth is the total number of bits needed
template<typename T>
inflection::dictionary::metadata::CompressedArray<T>::CompressedArray(int32_t wordWidth, int32_t arraySize)
    : dataArrayLength(((arraySize * wordWidth)+DATAWIDTH-1)/DATAWIDTH)
    , wordWidth(wordWidth)
    , data(new uint64_t[dataArrayLength])
    , selectMask((uint64_t(1) << wordWidth) - 1)
    , ownData(true)
{
    memset(data, 0, dataArrayLength * sizeof(data[0]));
}

template<typename T>
int8_t inflection::dictionary::metadata::CompressedArray<T>::calculateBitWidth(uint64_t allUsedBits)
{
    int8_t wordWidth = 0;
    while (allUsedBits != 0) {
        wordWidth++;
        allUsedBits >>= 1;
    }
    if (wordWidth == 0) {
        wordWidth = 1;
    }
    return wordWidth;
}

template<typename T>
int8_t inflection::dictionary::metadata::CompressedArray<T>::calculateBitWidth(const ::std::vector<T>& input)
{
    uint64_t allUsedBits = 0;
    for (T value : input) {
        allUsedBits |= ((uint64_t) value);
    }
    return calculateBitWidth(allUsedBits);
}

template<typename T>
T inflection::dictionary::metadata::CompressedArray<T>::extractValue(uint64_t value, uint8_t start, uint8_t len)
{
    if ((start + len) > 64) {
        throw inflection::exception::IndexOutOfBoundsException(u"Illegal start or length value");
    }
    return T((value >> start) & ((uint64_t(1) << len) - 1));
}

template<typename T>
inflection::dictionary::metadata::CompressedArray<T>::CompressedArray(const ::std::vector<T>& input)
    : CompressedArray(calculateBitWidth(input), int32_t(input.size()))
{
    const int32_t arraySize = (int32_t)input.size();
    for (int32_t i = 0; i < arraySize; i++) {
        write(i, input[i]);
    }
}

template<typename T>
inflection::dictionary::metadata::CompressedArray<T>::~CompressedArray() {
    if (ownData) {
        delete[] data;
    }
}

template <typename T>
bool inflection::dictionary::metadata::CompressedArray<T>::isEmpty() const
{
    return dataArrayLength == 0;
}

template <typename T>
T inflection::dictionary::metadata::CompressedArray<T>::read(int32_t index) const{
    int32_t startBitPos = index * wordWidth;
    int32_t shiftStart = startBitPos % DATAWIDTH;
    auto currData = data + (startBitPos / DATAWIDTH);
    auto endData = data + ((startBitPos + (wordWidth - 1)) / DATAWIDTH);

    if (index < 0 || endData > data + dataArrayLength) {
        throw inflection::exception::IndexOutOfBoundsException(u"Invalid CompressedArray index");
    }

    uint64_t retVal = (selectMask & (*currData >> shiftStart));
    if (currData++ != endData) {
        retVal |= (selectMask & (*currData << (DATAWIDTH - shiftStart)));
    }
    return ((T) retVal);
}

template <typename T>
void inflection::dictionary::metadata::CompressedArray<T>::write(int32_t index, T value) {
    uint64_t unsignedVal = (uint64_t) value;

    int32_t startBitPos = index * wordWidth;
    int32_t shiftStart = startBitPos % DATAWIDTH;
    auto currData = data + (startBitPos / DATAWIDTH);
    auto endData = data + ((startBitPos + (wordWidth - 1)) / DATAWIDTH);

    if (endData > data + dataArrayLength) {
        throw inflection::exception::IndexOutOfBoundsException(u"CompressedArray index too big");
    }

    *currData |= (unsignedVal << shiftStart);
    if (currData++ != endData) {
        *currData |= (unsignedVal >> (DATAWIDTH - shiftStart));
    }
}

template <typename T>
void inflection::dictionary::metadata::CompressedArray<T>::serialize(::std::ostream& writer) const
{
    writer.write(reinterpret_cast<const char*>(&dataArrayLength), sizeof(dataArrayLength));
    writer.write(reinterpret_cast<const char*>(&wordWidth), sizeof(wordWidth));
    writer.write(reinterpret_cast<const char*>(data), sizeof(data[0]) * dataArrayLength);
}
