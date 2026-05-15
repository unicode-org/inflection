/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/dictionary/metadata/CharsetConverter.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/dictionary/metadata/MarisaTrieIterator.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <marisa/trie.h>
#include <marisa/iostream.h>
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>

/**
 * Maps a const char16_t* to an arbitrary POD type.
 * The string is looked up in a Marisa trie, which yields a key.
 * The output is stored at this offset in the value storage.
 */
template <typename T>
class inflection::dictionary::metadata::MarisaTrie {
public:
    typedef enum EncodingEnum {
        BOCU1,
        UTF8,
        UTF16LE,
    } EncodingEnum;

    static inline const char * getEncodingName(EncodingEnum encodingEnum) {
        switch (encodingEnum) {
            case BOCU1:
                return "BOCU1";
            case UTF8:
                return "UTF8";
            case UTF16LE:
                return "UTF16LE";
            default:
                throw ::inflection::exception::IllegalStateException(u"Unknown encoding value " + ::inflection::util::StringUtils::to_u16string(encodingEnum));
        }
    }

    T find(int32_t id) const;
    std::optional<T> find(std::u16string_view key) const;
    int32_t getKeyId(std::u16string_view key) const;
    ::std::u16string getKey(int32_t id) const;

    inflection::dictionary::metadata::MarisaTrieIterator<T> getAllWithPrefix(std::u16string_view prefix) const;

    int32_t getSize() const;

    void write(::std::ostream& writer) const;

    explicit MarisaTrie(const ::std::map<::std::u16string_view, T>& input);
    explicit MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile);
    ~MarisaTrie();

private:
    ::marisa::Trie trie {  };
    ::inflection::dictionary::metadata::CharsetConverter encoder;
    CompressedArray<T> data;
    int8_t encodingEnum {  };

    typedef struct FieldMetrics {
        EncodingEnum keyEncoding {  };
        int16_t wordWidth {  };
    } FieldMetrics;
    /** When you get over 128K entries, sample a minimum of 64K entries */
    static constexpr uint32_t SHIFT_AMOUNT_FOR_SKIPPING = 17;

    MarisaTrie(const ::std::map<::std::u16string_view, T>& input, FieldMetrics fieldMetrics);
    MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile, int32_t trieSize);
    MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile, int32_t trieSize, EncodingEnum encodingEnum);
    MarisaTrie(const MarisaTrie& other) = delete;
    MarisaTrie& operator=(const MarisaTrie& other) = delete;

    static inline FieldMetrics getFieldMetrics(const ::std::map<::std::u16string_view, T>& input);

    friend class MarisaTrieIterator<T>;
    friend class StringContainer;
};

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::MarisaTrie(const ::std::map<::std::u16string_view, T>& input, FieldMetrics fieldMetrics)
    : encoder(getEncodingName(fieldMetrics.keyEncoding))
    , data(fieldMetrics.wordWidth, int32_t(input.size()))
    , encodingEnum(fieldMetrics.keyEncoding)
{
    ::marisa::Keyset keyset;
    ::std::string encoded;
    ::std::u16string_view keyString;

    // Create the encoded keys
    for (auto entry : input) {
        keyString = entry.first;
        encoder.encode(&encoded, keyString);
        keyset.push_back(encoded.data(), encoded.length());
    }
    trie.build(keyset);

    // We iterate over the map again. The Keyset indexes are the same, but the keyset has had its id updated.
    size_t keyIdx = 0;
    for (const auto& entry : input) {
        data.write((int32_t) keyset[keyIdx++].id(), entry.second);
    }
}

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::MarisaTrie(const ::std::map<::std::u16string_view, T>& input)
    : MarisaTrie(input, getFieldMetrics(input))
{
}

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile, int32_t trieSize, EncodingEnum encodingEnum)
    : encoder(getEncodingName(encodingEnum))
    , data(mappedFile)
    , encodingEnum(encodingEnum)
{
    if (trieSize > 0) {
        const char* rawTrieData = npc(mappedFile)->readArray<char>(trieSize);
        trie.map(rawTrieData, trieSize);
    }
    else {
        // Create an empty trie that you can get a valid key iterator.
        ::marisa::Keyset keyset;
        trie.build(keyset);
    }
}

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile, int32_t trieSize)
    : MarisaTrie(mappedFile, trieSize, (EncodingEnum)util::Validate::notNull(mappedFile)->read<int16_t>())
{
}

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::MarisaTrie(::inflection::util::MemoryMappedFile* mappedFile)
    : MarisaTrie(mappedFile, util::Validate::notNull(mappedFile)->read<int32_t>())
{
}

template <typename T>
inflection::dictionary::metadata::MarisaTrie<T>::~MarisaTrie()
{
}

template<typename T>
typename inflection::dictionary::metadata::MarisaTrie<T>::FieldMetrics
inflection::dictionary::metadata::MarisaTrie<T>::getFieldMetrics(const std::map<::std::u16string_view, T> &input) {
    // Find the most efficient encoding
    inflection::dictionary::metadata::CharsetConverter encodeBOCU1(getEncodingName(BOCU1));
    ::std::string encoded;
    size_t lengthBOCU1 = 0;
    size_t lengthUTF8 = 0;
    size_t lengthUTF16 = 0;
    uint64_t allUsedBits = 0;
    auto skipEntries = input.size() >> SHIFT_AMOUNT_FOR_SKIPPING;
    uint32_t entryCount = 0;
    for (const auto& [key, bits] : input) {
        allUsedBits |= bits;
        if (skipEntries != 0) {
            if (entryCount++ < skipEntries) {
                continue; // We have a large number of keys. Skip a few.
            }
            entryCount = 0;
        }
        lengthUTF16 += key.length();
        encodeBOCU1.encode(&encoded, key);
        lengthBOCU1 += encoded.length();
        ::inflection::util::StringViewUtils::convert(&encoded, key);
        lengthUTF8 += encoded.length();
    }
    lengthUTF16 *= sizeof(char16_t);
    EncodingEnum encodingEnum;

    if (lengthBOCU1 < lengthUTF8 && lengthBOCU1 < lengthUTF16) {
        encodingEnum = BOCU1;
    }
    else if (lengthUTF16 < lengthUTF8 && lengthUTF16 < lengthBOCU1) {
        encodingEnum = UTF16LE;
    }
    else {
        // When all things are equal for size, this conversion is faster than BOCU1 by 2-5% on average.
        encodingEnum = UTF8;
    }

    // Calculate the maximum number of bits needed to store a data value array
    int16_t wordWidth = 0;
    while (allUsedBits != 0) {
        wordWidth++;
        allUsedBits >>= 1;
    }
    wordWidth += int8_t(wordWidth == 0);

    return {encodingEnum, wordWidth};
}

template <typename T>
::std::u16string inflection::dictionary::metadata::MarisaTrie<T>::getKey(int32_t id) const
{
    ::std::u16string result;
    ::marisa::Agent agent;
    agent.set_query(id);
    trie.reverse_lookup(agent);
    encoder.decode(&result, agent.key().ptr(), int32_t(agent.key().length()));
    return result;
}

template <typename T>
int32_t inflection::dictionary::metadata::MarisaTrie<T>::getKeyId(std::u16string_view key) const
{
    ::std::string encoded;
    ::marisa::Agent agent;
    encoder.encode(&encoded, key);
    agent.set_query(encoded.data(), encoded.length());
    if (!trie.lookup(agent)) {
        return -1;
    }
    return (int32_t)agent.key().id();
}

template <typename T>
T inflection::dictionary::metadata::MarisaTrie<T>::find(int32_t id) const
{
    return data.read(id);
}

template <typename T>
std::optional<T> inflection::dictionary::metadata::MarisaTrie<T>::find(std::u16string_view key) const
{
    auto id = getKeyId(key);
    if (id < 0) {
        return {};
    }
    return data.read(id);
}

template <typename T>
void inflection::dictionary::metadata::MarisaTrie<T>::write(::std::ostream& writer) const
{
    // We're writing 2 32-bit numbers to keep 64-bit alignment.
    std::ostringstream buffer;
    int32_t trieSize = 0;
    int16_t encodingEnumValue = encodingEnum;
    if (!trie.empty()) {
        buffer << trie;
        trieSize = (int32_t)buffer.tellp();
    }

    writer.write(reinterpret_cast<const char*>(&trieSize), sizeof(trieSize));
    writer.write(reinterpret_cast<const char*>(&encodingEnumValue), sizeof(encodingEnumValue));
    data.serialize(writer);
    if (trieSize > 0) {
        writer.write(buffer.str().data(), trieSize);
    }
}

template <typename T>
inflection::dictionary::metadata::MarisaTrieIterator<T>
inflection::dictionary::metadata::MarisaTrie<T>::getAllWithPrefix(std::u16string_view prefix) const
{
    return MarisaTrieIterator<T>(*this, prefix);
}

template <typename T>
int32_t inflection::dictionary::metadata::MarisaTrie<T>::getSize() const
{
    return trie.size();
}
