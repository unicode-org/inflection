/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/dictionary/metadata/StringContainer.hpp>
#include <inflection/dictionary/metadata/StringArrayContainer.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/util/Finally.hpp>
#include <inflection/util/MemoryMappedFile.hpp>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>

template<typename A, typename B>
bool compare(A a, B b) {
    return a == b;
}

bool compare(const ::std::vector<::std::u16string>& a, const ::std::vector<::std::u16string_view>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    int32_t aSize = (int32_t)a.size();
    for (int32_t i = 0; i < aSize; ++i) {
        if (!compare(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

bool compare(const ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>>& a, const ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (const auto& entry : a) {
        auto equivalent = b.find(entry.first);
        if (equivalent == b.end()) {
            return false;
        }
        if (!compare(entry.second, equivalent->second)) {
            return false;
        }
    }
    return true;
}

template <typename A, typename B>
void ensureEquivalence(A mapA, B mapB)
{
    for (const auto& entry : mapA) {
        auto lookup = mapB->find(entry.first);
        REQUIRE(lookup);
        REQUIRE(compare(*lookup, entry.second));
    }
}

static ::std::filesystem::path createTemporaryFilePath()
{
    return ::std::filesystem::temp_directory_path() / "marisaTrieTest";
}

TEST_CASE("MMappedDictionaryTest#testTrie")
{
    ::std::map<::std::u16string_view, int64_t> wordsToTypes({
        {u"abc", 292},
        {u"def", 42},
        {u"xyz", 1000}
    });

    // Trie creation from map
    auto trie = new inflection::dictionary::metadata::MarisaTrie<int64_t>(wordsToTypes);
    ensureEquivalence(wordsToTypes, trie);

    // Write out trie
    std::ostringstream buffer;
    REQUIRE_NOTHROW(trie->write(buffer));
    size_t trieSize = (size_t)buffer.tellp();
    delete trie;

    // Read in trie
    auto string = buffer.str();
    inflection::util::MemoryMappedFile mappedFile(string.data(), trieSize);
    trie = new inflection::dictionary::metadata::MarisaTrie<int64_t>(&mappedFile);
    ensureEquivalence(wordsToTypes, trie);
    delete trie;
    REQUIRE_THROWS(mappedFile.read<int64_t>());
}

TEST_CASE("MMappedDictionaryTest#testStringArrayContainer")
{
    std::set<std::u16string_view> testData({u"a",u"cde",u"b"});
    ::std::map<::std::u16string_view, int32_t> mappingData;
    inflection::dictionary::metadata::StringArrayContainer originalContainer(testData, &mappingData);
    std::ostringstream buffer;
    REQUIRE_NOTHROW(originalContainer.write(buffer));
    size_t size = (size_t)buffer.tellp();

    // Read in map
    auto string = buffer.str();
    inflection::util::MemoryMappedFile mappedFile(string.data(), size);
    inflection::dictionary::metadata::StringArrayContainer container(&mappedFile);
    CHECK(container.getIdentifier(u"a") == 0);
    CHECK(container.getIdentifier(u"b") == 1);
    CHECK(container.getIdentifier(u"cde") == 2);
    CHECK(container.getString(2) == u"cde");
    CHECK_THROWS(container.getIdentifier(u""));
    CHECK_THROWS(container.getIdentifier(u"missing"));
    CHECK_THROWS(container.getString(3));
    CHECK_THROWS(container.getString(-1));
}

TEST_CASE("MMappedDictionaryTest#testStringContainer")
{
    std::set<std::u16string_view> testData({u"a",u"cde",u"b"});
    ::std::map<::std::u16string_view, int32_t> mappingData;
    inflection::dictionary::metadata::StringContainer originalContainer(testData, &mappingData);
    std::ostringstream buffer;
    REQUIRE_NOTHROW(originalContainer.write(buffer));
    size_t size = (size_t)buffer.tellp();

    // Read in map
    auto string = buffer.str();
    inflection::util::MemoryMappedFile mappedFile(string.data(), size);
    inflection::dictionary::metadata::StringContainer container(&mappedFile);
    CHECK(container.getIdentifier(u"a") == 0);
    CHECK(container.getIdentifier(u"b") == 1);
    CHECK(container.getIdentifier(u"cde") == 2);
    CHECK(container.getString(2) == u"cde");
    CHECK_THROWS(container.getIdentifier(u""));
    CHECK_THROWS(container.getIdentifier(u"missing"));
    CHECK_THROWS(container.getString(3));
    CHECK_THROWS(container.getString(-1));
    CHECK(container.size() == 3);
}

TEST_CASE("MMappedDictionaryTest#testCompressedArray")
{
    std::vector<int32_t> testData({1,3,2});
    inflection::dictionary::metadata::CompressedArray<int32_t> compressedArray(testData);
    CHECK_THROWS(compressedArray.read(-1));
    CHECK(compressedArray.read(0) == 1);
    CHECK(compressedArray.read(1) == 3);
    CHECK(compressedArray.read(2) == 2);
    CHECK_THROWS(compressedArray.read(64));
}

TEST_CASE("MMappedDictionaryTest#readInvalidFiles")
{
    auto temporaryPath = createTemporaryFilePath();
    ::inflection::util::Finally finally([&temporaryPath]() noexcept {
        ::std::filesystem::remove(temporaryPath);
    });

    ::std::u16string uPath;
    ::inflection::util::StringUtils::convert(&uPath, temporaryPath.string());

    // Write the full file contents and close the handle before each createDictionary() call below,
    // since a still-open writer handle causes CreateFileW to fail with a sharing violation on Windows.
    ::std::string contents(::inflection::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER,
            sizeof(::inflection::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER));
    auto writeContents = [&]() {
        ::std::ofstream file(temporaryPath, ::std::ios::binary | ::std::ios::trunc);
        REQUIRE(file.is_open());
        file.write(contents.data(), (std::streamsize)contents.size());
        REQUIRE(file.good());
    };

    writeContents();
    REQUIRE_THROWS_AS(inflection::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
            inflection::exception::IOException);

    // version off by 1
    int16_t endiannessMarker = 1;
    auto incompatibleVersion = inflection::dictionary::DictionaryMetaData_MMappedDictionary::VERSION + 1;
    contents.append(reinterpret_cast<const char*>(&incompatibleVersion), sizeof(incompatibleVersion));
    contents.append(reinterpret_cast<const char*>(&endiannessMarker), sizeof(endiannessMarker));
    writeContents();

    // header now valid, but version incompatible
    REQUIRE_THROWS_AS(inflection::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
            inflection::exception::IncompatibleVersionException);

    // invalid endianness marker
    auto validVersion = inflection::dictionary::DictionaryMetaData_MMappedDictionary::VERSION;
    auto versionOffset = sizeof(::inflection::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER);
    ::std::memcpy(&contents[versionOffset], &validVersion, sizeof(validVersion));
    int16_t invalidEndiannessMarker = 0xFF;
    ::std::memcpy(&contents[versionOffset + sizeof(validVersion)], &invalidEndiannessMarker, sizeof(invalidEndiannessMarker));
    writeContents();

    REQUIRE_THROWS_AS(inflection::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
                      inflection::exception::IOException);
    REQUIRE_THROWS_AS(inflection::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(u"."),
                      inflection::exception::IOException);
}
