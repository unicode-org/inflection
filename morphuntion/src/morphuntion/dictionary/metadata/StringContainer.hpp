/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/metadata/fwd.hpp>
#include <morphuntion/dictionary/metadata/CharsetConverter.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <marisa/trie.h>

#include <map>
#include <set>
#include <ostream>

/**
 * A trie of strings. Compression is involved by using a trie. This is better for large quantities of strings.
 */
class MORPHUNTION_INTERNAL_API morphuntion::dictionary::metadata::StringContainer {
private:
    int8_t encodingEnum {  };
    marisa::Trie trie {  };
    ::morphuntion::dictionary::metadata::CharsetConverter encoder;
    int32_t _size = 0;

public:
    void write(::std::ostream& output) const;

    ::std::u16string getString(int32_t identifier) const;
    /** Return -1 if not present */
    int32_t getIdentifierIfAvailable(::std::u16string_view string) const;
    /** Throw an exception if not present */
    int32_t getIdentifier(::std::u16string_view string) const;

    static int64_t convertIdentifierToBit(int32_t identifier);
    static int8_t convertBitToIdentifier(int64_t bit);

    //Returns the number of strings in the container
    int32_t size() const;

    explicit StringContainer(morphuntion::util::MemoryMappedFile *mappedFile);
    explicit StringContainer(const ::std::set<::std::u16string_view>& strings, ::std::map<::std::u16string_view, int32_t>* mappingResult);
    StringContainer();
    ~StringContainer();
private:
    StringContainer(StringContainer& other) = delete;
};
