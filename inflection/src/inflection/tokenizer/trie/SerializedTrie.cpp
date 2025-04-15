/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/trie/SerializedTrie.hpp>

#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IOException.hpp>
#include <cstring>

namespace inflection::tokenizer::trie {

constexpr int64_t SerializedTrie::VERSION;
constexpr char SerializedTrie::MAGIC_MARKER[8];
static constexpr int32_t MAGIC_MARKER_LEN = sizeof(SerializedTrie::MAGIC_MARKER);
static constexpr int16_t ENDIANNESS_MARKER = 1;

SerializedTrie::SerializedTrie(const std::u16string &corpusFile)
    : mappedFile(corpusFile)
    , trie(&validateHeader(mappedFile, corpusFile))
{
}

SerializedTrie::~SerializedTrie()
{
}

::inflection::util::MemoryMappedFile& SerializedTrie::validateHeader(::inflection::util::MemoryMappedFile& mappedFile, const std::u16string &corpusFile)
{
    // Verify header starts with magic token
    const char* magicMarker;
    mappedFile.read(&magicMarker, MAGIC_MARKER_LEN);
    if (strncmp(magicMarker, MAGIC_MARKER, MAGIC_MARKER_LEN) != 0) {
        throw ::inflection::exception::IOException(u"Input file " + corpusFile + u" has an invalid header");
    }

    if (mappedFile.read<int64_t>() != VERSION) {
        throw ::inflection::exception::IncompatibleVersionException(u"Input file " + corpusFile + u" has an incompatible version");
    }

    if (mappedFile.read<int16_t>() != ENDIANNESS_MARKER) {
        throw ::inflection::exception::IOException(u"Input file " + corpusFile + u" was built for a different architecture");
    }
    return mappedFile;
}

} // namespace inflection::tokenizer::trie
