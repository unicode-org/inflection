/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/util/MemoryMappedFile.hpp>

#include <cstdint>
#include <map>
#include <set>
#include <ostream>

/**
 * A simple array of strings. No compression is involved. This is smaller and faster for small quantities of strings.
 */
class INFLECTION_INTERNAL_API inflection::dictionary::metadata::StringArrayContainer {
private:
    int32_t arraySize {  };
    int32_t allStringsSize {  };
    int32_t* stringIndexesWithLen {  };
    char16_t* allStrings {  };
    bool ownData {  };

public:
    void write(::std::ostream& output) const;

    ::std::u16string getString(int32_t identifier) const;
    /** Return -1 if not present */
    int32_t getIdentifierIfAvailable(::std::u16string_view string) const;
    /** Throw an exception if not present */
    int32_t getIdentifier(::std::u16string_view string) const;

    //Returns the number of strings in the container
    int32_t size() const;

    explicit StringArrayContainer(inflection::util::MemoryMappedFile *mappedFile);
    explicit StringArrayContainer(const ::std::set<::std::u16string_view>& strings, ::std::map<::std::u16string_view, int32_t>* mappingResult);
    StringArrayContainer();
    ~StringArrayContainer();
private:
    StringArrayContainer(StringArrayContainer& other) = delete;
    StringArrayContainer& operator=(const StringArrayContainer& other) = delete;
};
