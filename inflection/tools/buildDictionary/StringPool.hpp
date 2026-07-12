/*
 * Copyright 2026-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

class StringPool {
    std::unordered_set<std::u16string> storage;
public:
    StringPool() {
        storage.max_load_factor(0.75f);
    }

    std::u16string_view intern(const std::u16string& str) {
        return *storage.insert(str).first;
    }

    std::u16string_view intern(std::u16string_view str) {
        return *storage.emplace(str).first;
    }

    /**
     * Look up a string without inserting it.
     * Returns a view into the pooled string, or empty optional if not found.
     */
    std::optional<std::u16string_view> find(std::u16string_view str) const {
        auto it = storage.find(std::u16string(str));
        if (it == storage.end()) {
            return std::nullopt;
        }
        return *it;
    }

    void reserve(size_t size) {
        storage.reserve(size);
    }

    size_t bucket_count() const {
        return storage.bucket_count();
    }
};
