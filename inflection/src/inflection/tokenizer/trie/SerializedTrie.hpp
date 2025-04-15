/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/trie/fwd.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/Object.hpp>

#include <string>
#include <string_view>

class inflection::tokenizer::trie::SerializedTrie final
    : public virtual ::inflection::Object
{
private:
    ::inflection::util::MemoryMappedFile mappedFile;

    static ::inflection::util::MemoryMappedFile& validateHeader(::inflection::util::MemoryMappedFile& mappedFile, const std::u16string &corpusFile);

public:
    const inflection::dictionary::metadata::MarisaTrie<int32_t> trie;
    static constexpr int64_t VERSION { 2 }; // Bump this version if the binary file format changes.
    static constexpr char MAGIC_MARKER[8] { "MORPHTK" };

    explicit SerializedTrie(const std::u16string &corpusFile);
    ~SerializedTrie() override;
};
