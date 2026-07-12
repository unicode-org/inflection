/*
 * Copyright 2022-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/grammar/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <optional>
#include <string>

class inflection::grammar::BidirectionalStringMap
    : public inflection::Object
{
public:
    typedef inflection::Object super;

private:
    ::inflection::util::MemoryMappedFile mappedFile;
    int8_t numberOfTries {  };
    uint8_t options {  };
    ::icu4cxx::UnicodeSet sourceSet {  };
    ::icu4cxx::UnicodeSet targetSet {  };
    inflection::dictionary::metadata::MarisaTrie<int32_t> sourceTrie;
    inflection::dictionary::metadata::MarisaTrie<int32_t>* dynamicTrie {  };
    const inflection::dictionary::metadata::MarisaTrie<int32_t>& targetTrie;

public:
    /**
     * A set of bit options.
     */
    enum class Options {
        FROM_CASE_SENSITIVE = 1,
        TO_CASE_SENSITIVE = 1 << 1,
    };

private:
    static int8_t validateHeader(::inflection::util::MemoryMappedFile& mappedFile, const std::u16string& pathToFile);
    static uint8_t getOptions(::inflection::util::MemoryMappedFile& mappedFile);
    static std::u16string getFromUnicodeSetPattern(::inflection::util::MemoryMappedFile& mappedFile);
public:
    const ::icu4cxx::UnicodeSet& getSourceSet() const;
    const ::icu4cxx::UnicodeSet& getTargetSet() const;
    uint8_t getOptions() const;
    std::optional<std::u16string> findTarget(std::u16string_view sourceString) const;
    std::optional<std::u16string> findSource(std::u16string_view sourceString) const;
public:
    explicit BidirectionalStringMap(const std::u16string& pathToFile);
    ~BidirectionalStringMap() override;
    BidirectionalStringMap(const BidirectionalStringMap&) = delete;
    BidirectionalStringMap& operator=(const BidirectionalStringMap&) = delete;

public:
    static constexpr int64_t VERSION { 2 };
    static constexpr char MAGIC_MARKER[8] { "INFLTM" };
    static constexpr int32_t RESERVED_BYTES = 4;
};
