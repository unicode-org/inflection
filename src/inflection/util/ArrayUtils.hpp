/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <compare>
#include <iterator>
#include <string_view>

/**
 * @brief Provides utility functions for use arrays.
 * @details Provides utility functions for use arrays.
 */
class inflection::util::ArrayUtils {
private:
    template <typename T1, typename T2>
    static constexpr auto compareValues(const T1& a, const T2& b) {
        return a <=> b;
    }

    template <typename T>
    static constexpr std::strong_ordering compareValues(const T* a, const T* b) {
        return std::basic_string_view<T>(a) <=> std::basic_string_view<T>(b);
    }

    template <class ArrayType, typename FieldGetter, std::size_t length>
    static constexpr bool isSorted(const ArrayType (&array)[length], FieldGetter fieldGetter) {
        for (std::size_t i = 1; i < length; ++i) {
            if (compareValues(fieldGetter(array[i - 1]), fieldGetter(array[i])) != std::strong_ordering::less) {
                return false;
            }
        }
        return true;
    }

public:
    /**
     * An optimized search of a sorted unique set of values. The 3-way comparison operator is used to compare the values.
     * This is different from std::lower_bound because that function will keep comparing items until it precisely finds
     * the first item. This is generally bad for Big Theta of unique values where there is a statistical chance
     * of hitting the right item at any given midpoint. There is a 100% chance of finding the desired item at O(log n).
     * There is a 50% chance of finding the right item before the final comparison.
     * There is a 25% chance of finding the right item at 2 comparisons before the final comparison.
     * The statistical chance continues when finding a known item.
     * This is important where comparisons are expensive, and the item is likely to be found.
     * There is no performance difference for unknown items.
     *
     * Note that the array must be a uniquely sorted constexpr array.
     * @tparam array The sorted array to search (as a template parameter)
     * @tparam KeyType The type of the value to search for in the array.
     * @tparam FieldGetter The type for fieldGetter.
     * @param value The value to search for in the array.
     * @param fieldGetter A callable that extracts the comparable field from an array element
     * @return nullptr when not found, or the item found when the extracted field equals the value
     */
    template <const auto& array, class KeyType, typename FieldGetter>
    static constexpr auto* searchSorted(const KeyType& value, FieldGetter fieldGetter) {
        using ArrayRefType = decltype(array);
        using ArrayType = std::remove_cvref_t<ArrayRefType>;
        using ElementType = std::remove_extent_t<ArrayType>;
        constexpr std::size_t length = std::extent_v<ArrayType>;
        static_assert(isSorted(array, fieldGetter), "Array must be sorted");

        std::size_t len = length;
        const ElementType* first = array;

        while (len != 0) {
            auto lenHalf = len / 2;
            const ElementType* mid = first;
            std::advance(mid, lenHalf);
            auto cmp = compareValues(value, fieldGetter(*mid));
            if (cmp == std::strong_ordering::greater) {
                first = mid + 1;
                len -= lenHalf + 1;
            } else if (cmp == std::strong_ordering::less) {
                len = lenHalf;
            }
            else {
                return mid;
            }
        }
        return static_cast<const ElementType *>(nullptr);
    }

    /**
     * Returns true if the value is contained in a sorted unique set of values, where the array elements are compared directly.
     * @tparam array The sorted array to search (as a template parameter)
     * @tparam KeyType The type of the value to search for in the array.
     * @param value The value to search for in the array.
     * @return true when the value is found in the array, false otherwise
     */
    template <const auto& array, class KeyType>
    static constexpr bool contains(const KeyType& value) {
        return searchSorted<array>(value, [](auto item) { return item; }) != nullptr;
    }

    ArrayUtils() = delete;
};
