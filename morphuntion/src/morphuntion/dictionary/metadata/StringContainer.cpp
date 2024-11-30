/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/metadata/StringContainer.hpp>

#include <morphuntion/dictionary/metadata/MarisaTrie.hpp>
#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <marisa/iostream.h>
#include <morphuntion/npc.hpp>
#include <cstring>

namespace morphuntion::dictionary::metadata {

StringContainer::StringContainer(::morphuntion::util::MemoryMappedFile *mappedFile)
    : encodingEnum(mappedFile->read<int8_t>())
    , encoder(MarisaTrie<int8_t>::getEncodingName((MarisaTrie<int8_t>::EncodingEnum)encodingEnum))
{
    mappedFile->read<int8_t>(); // padding reserved for future use.
    mappedFile->read<int8_t>(); // padding reserved for future use.
    mappedFile->read<int8_t>(); // padding reserved for future use.

    int32_t trieSize;
    mappedFile->read(&trieSize);
    if (trieSize > 0) {
        // non-empty trie to read.
        char* rawTrieData;
        mappedFile->read(&rawTrieData, trieSize);

        trie.map(rawTrieData, trieSize);
        _size = (int32_t) trie.num_keys();
    }
}

static MarisaTrie<int8_t>::EncodingEnum getEncoding(const ::std::set<::std::u16string_view>& data) {
    // Find the most efficient encoding
    CharsetConverter encodeBOCU1(MarisaTrie<int8_t>::getEncodingName(MarisaTrie<int8_t>::BOCU1));
    ::std::string encoded;
    size_t lengthBOCU1 = 0;
    size_t lengthUTF8 = 0;
    size_t lengthUTF16 = 0;
    for (const auto& keyString : data) {
        lengthUTF16 += keyString.length();
        encodeBOCU1.encode(&encoded, keyString);
        lengthBOCU1 += encoded.length();
        ::morphuntion::util::StringViewUtils::convert(&encoded, keyString);
        lengthUTF8 += encoded.length();
    }
    lengthUTF16 *= sizeof(char16_t);
    MarisaTrie<int8_t>::EncodingEnum encodingEnum;

    if (lengthBOCU1 < lengthUTF8 && lengthBOCU1 < lengthUTF16) {
        encodingEnum = MarisaTrie<int8_t>::BOCU1;
    }
    else if (lengthUTF16 < lengthUTF8 && lengthUTF16 < lengthBOCU1) {
        encodingEnum = MarisaTrie<int8_t>::UTF16LE;
    }
    else {
        // When all things are equal for size, this conversion is faster than BOCU1 by 2-5% on average.
        encodingEnum = MarisaTrie<int8_t>::UTF8;
    }
    return encodingEnum;
}

StringContainer::StringContainer(const ::std::set<std::u16string_view>& data, ::std::map<std::u16string_view, int32_t>* mappingResult)
    : encodingEnum(getEncoding(data))
    , encoder(MarisaTrie<int8_t>::getEncodingName((MarisaTrie<int8_t>::EncodingEnum)encodingEnum))
    , _size(int32_t(data.size()))
{
    morphuntion::util::Validate::isTrue(npc(mappingResult)->empty(), u"The mapping result should be empty");

    ::marisa::Keyset keyset;
    ::std::string encoded;

    // Create the encoded keys
    for (const auto& keyString : data) {
        encoder.encode(&encoded, keyString);
        keyset.push_back(encoded.data(), encoded.length());
    }
    trie.build(keyset);
    morphuntion::util::Validate::isTrue(((int32_t) trie.num_keys()) == _size, u"Number of keys in string container should match with size of data");

    size_t keySetIdx = 0;
    for (const auto& keyString : data) {
        npc(mappingResult)->emplace(keyString, keyset[keySetIdx++].id());
    }
}

StringContainer::StringContainer()
    : encodingEnum(MarisaTrie<int8_t>::EncodingEnum::UTF8)
    , encoder(MarisaTrie<int8_t>::getEncodingName((MarisaTrie<int8_t>::EncodingEnum)encodingEnum))
{
}

StringContainer::~StringContainer() = default;

void StringContainer::write(::std::ostream& output) const
{
    int8_t padding = 0;
    output.write(reinterpret_cast<const char*>(&encodingEnum), sizeof(encodingEnum));
    output.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    output.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    output.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    if (trie.empty()) {
        // A zero size trie is still 3432 bytes. Let's not write that.
        int32_t trieSize = 0;
        output.write(reinterpret_cast<const char*>(&trieSize), sizeof(trieSize));
    }
    else {
        std::ostringstream buffer;
        buffer << trie;
        int32_t trieSize = (int32_t)buffer.tellp();
        output.write(reinterpret_cast<const char*>(&trieSize), sizeof(trieSize));
        output.write(buffer.str().c_str(), trieSize);
    }
}

::std::u16string StringContainer::getString(int32_t identifier) const
{
    ::std::u16string result;
    ::marisa::Agent agent;
    agent.set_query(identifier);
    trie.reverse_lookup(agent);
    encoder.decode(&result, agent.key().ptr(), int32_t(agent.key().length()));
    return result;
}

int32_t StringContainer::getIdentifierIfAvailable(::std::u16string_view string) const
{
    ::std::string encoded;
    encoded.reserve(string.length());
    ::marisa::Agent agent;
    encoder.encode(&encoded, string);
    agent.set_query(encoded.c_str(), encoded.length());
    if (!trie.lookup(agent)) {
        return -1;
    }
    return int32_t(agent.key().id());
}

int32_t StringContainer::getIdentifier(::std::u16string_view string) const
{
    auto result = getIdentifierIfAvailable(string);
    if (result < 0) {
        throw ::morphuntion::exception::IllegalArgumentException(::std::u16string(u"String not found: ") + ::std::u16string(string));
    }
    return result;
}

int64_t StringContainer::convertIdentifierToBit(int32_t identifier)
{
    return int64_t(1) << identifier;
}

static int8_t getIndexFromBit(uint64_t value)
{
    if (value == 0 || ((value - 1) & value) != 0) {
        // There isn't exactly 1 bit on, which is an error
        return -1;
    }
    return std::popcount(value - 1);
}

int8_t StringContainer::convertBitToIdentifier(int64_t bit)
{
    return getIndexFromBit(bit);
}

int32_t StringContainer::size() const {
    return _size;
}

} // namespace morphuntion::dictionary::metadata
