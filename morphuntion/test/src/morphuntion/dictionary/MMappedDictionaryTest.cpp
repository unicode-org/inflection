/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dictionary/metadata/CompressedArray.hpp>
#include <morphuntion/dictionary/metadata/StringContainer.hpp>
#include <morphuntion/dictionary/metadata/StringArrayContainer.hpp>
#include <morphuntion/dictionary/metadata/MarisaTrie.hpp>
#include <morphuntion/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/exception/IncompatibleVersionException.hpp>
#include <morphuntion/exception/IOException.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>

#include <map>
#include <unistd.h>
#include <string.h>

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

::std::string createTemporaryFileTemplate()
{
    const char* tmpDir = nullptr;
    // temporary folder lookup according to ISO/IEC 9945
    for (auto reference : ::std::vector<const char*> { "TMPDIR", "TMP", "TEMP", "TEMPDIR" }) {
        tmpDir = ::std::getenv(reference);
        if (tmpDir != nullptr) {
            break;
        }
    }
    if (tmpDir == nullptr) {
        tmpDir = "/tmp";
    }
    return ::std::string(tmpDir) + "/marisaTrieTestXXXXXX";
}

TEST_CASE("MMappedDictionaryTest#testTrie")
{
    ::std::map<::std::u16string_view, int64_t> wordsToTypes({
        {u"abc", 292},
        {u"def", 42},
        {u"xyz", 1000}
    });

    // Trie creation from map
    auto trie = new morphuntion::dictionary::metadata::MarisaTrie<int64_t>(wordsToTypes);
    ensureEquivalence(wordsToTypes, trie);

    // Write out trie
    std::ostringstream buffer;
    REQUIRE_NOTHROW(trie->write(buffer));
    size_t trieSize = (size_t)buffer.tellp();
    delete trie;

    // Read in trie
    auto string = buffer.str();
    morphuntion::util::MemoryMappedFile mappedFile(string.data(), trieSize);
    trie = new morphuntion::dictionary::metadata::MarisaTrie<int64_t>(&mappedFile);
    ensureEquivalence(wordsToTypes, trie);
    delete trie;
    REQUIRE_THROWS(mappedFile.read<int64_t>());
}

TEST_CASE("MMappedDictionaryTest#testStringArrayContainer")
{
    std::set<std::u16string_view> testData({u"a",u"cde",u"b"});
    ::std::map<::std::u16string_view, int32_t> mappingData;
    morphuntion::dictionary::metadata::StringArrayContainer originalContainer(testData, &mappingData);
    std::ostringstream buffer;
    REQUIRE_NOTHROW(originalContainer.write(buffer));
    size_t size = (size_t)buffer.tellp();

    // Read in map
    auto string = buffer.str();
    morphuntion::util::MemoryMappedFile mappedFile(string.data(), size);
    morphuntion::dictionary::metadata::StringArrayContainer container(&mappedFile);
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
    morphuntion::dictionary::metadata::StringContainer originalContainer(testData, &mappingData);
    std::ostringstream buffer;
    REQUIRE_NOTHROW(originalContainer.write(buffer));
    size_t size = (size_t)buffer.tellp();

    // Read in map
    auto string = buffer.str();
    morphuntion::util::MemoryMappedFile mappedFile(string.data(), size);
    morphuntion::dictionary::metadata::StringContainer container(&mappedFile);
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
    morphuntion::dictionary::metadata::CompressedArray compressedArray(testData);
    CHECK_THROWS(compressedArray.read(-1));
    CHECK(compressedArray.read(0) == 1);
    CHECK(compressedArray.read(1) == 3);
    CHECK(compressedArray.read(2) == 2);
    CHECK_THROWS(compressedArray.read(64));
}

TEST_CASE("MMappedDictionaryTest#readInvalidFiles")
{
    auto temporaryPath = createTemporaryFileTemplate();
    int temporaryFile = mkstemp(&temporaryPath[0]);
    REQUIRE(temporaryFile > 0);
    ::std::u16string uPath;
    ::morphuntion::util::StringUtils::convert(&uPath, temporaryPath);

    REQUIRE(sizeof(::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER)
            == write(temporaryFile, ::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER,
            sizeof(::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER)));
    fsync(temporaryFile);
    REQUIRE_THROWS_AS(morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
            morphuntion::exception::IOException);

    // version off by 1
    int16_t endiannessMarker = 1;
    auto incompatibleVersion = morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::VERSION + 1;
    REQUIRE(sizeof(endiannessMarker) == write(temporaryFile, &endiannessMarker, sizeof(endiannessMarker)));
    REQUIRE(sizeof(incompatibleVersion) == write(temporaryFile, &incompatibleVersion, sizeof(incompatibleVersion)));
    fsync(temporaryFile);

    // header now valid, but version incompatible
    REQUIRE_THROWS_AS(morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
            morphuntion::exception::IncompatibleVersionException);

    // invalid endianness marker
    lseek(temporaryFile, strlen(::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::MAGIC_MARKER), SEEK_SET);
    int16_t invalidEndiannessMarker = 0xFF;
    REQUIRE(sizeof(invalidEndiannessMarker) == write(temporaryFile, &invalidEndiannessMarker, sizeof(invalidEndiannessMarker)));
    fsync(temporaryFile);

    REQUIRE_THROWS_AS(morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(uPath),
                      morphuntion::exception::IOException);
    REQUIRE_THROWS_AS(morphuntion::dictionary::DictionaryMetaData_MMappedDictionary::createDictionary(u"."),
                      morphuntion::exception::IOException);

    close(temporaryFile);
    unlink(temporaryPath.c_str());
}
