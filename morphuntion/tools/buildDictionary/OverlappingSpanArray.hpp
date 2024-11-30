/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <algorithm>
#include <map>
#include <vector>

/**
 * This class creates a vector of values. If the vector already has the values, getOffset will return the starting index
 * to the singletons vector. When the vector requested from getOffset doesn't exist, it will be appended
 * to the singletons vector.
 *
 * Here are examples:
 * singletons: 10,20,30,40
 * getOffset({20,30}) returns 1
 * singletons: 10,20,30,40
 * getOffset({6,5}) returns 4 and adds 6,5 to the end
 *
 * @tparam VecType A numerical value
 */
template<typename VecType>
class OverlappingSpanArray final
{
private:
    std::vector<VecType> singletons {  };

public:
    /**
     * Get the offset into the singletons array. If it doesn't exist, add it to the end.
     */
    inline int32_t getOffset(const std::vector<VecType>& subset);
    inline const std::vector<VecType>& getSingletons() const;
    inline void reserve(size_t size);

public:
    OverlappingSpanArray() = default;
};

template<typename VecType>
inline int32_t OverlappingSpanArray<VecType>::getOffset(const std::vector<VecType>& subset)
{
    int32_t offset = 0;
    if (!subset.empty()) {
        auto subsetStart = std::search(singletons.begin(), singletons.end(), subset.begin(), subset.end());
        if (subsetStart == singletons.end()) {
            offset = int32_t(singletons.size());
            singletons.insert(singletons.end(), subset.begin(), subset.end());
        }
        else {
            offset = int32_t(subsetStart - singletons.begin());
        }
    }

    return offset;
}

template<typename VecType>
inline const std::vector<VecType>& OverlappingSpanArray<VecType>::getSingletons() const
{
    return singletons;
}

template<typename VecType>
inline void OverlappingSpanArray<VecType>::reserve(size_t size)
{
    singletons.reserve(size);
}
