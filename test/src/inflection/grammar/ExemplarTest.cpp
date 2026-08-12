/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/grammar/BidirectionalStringMap.hpp>
#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <filesystem>
#include <memory>

#include "util/TestUtils.hpp"

#ifndef INFLECTION_DATA_ROOT_DIR
#define INFLECTION_DATA_ROOT_DIR "/usr/local/share"
#endif

static constexpr int32_t MAGIC_MARKER_LEN = sizeof(inflection::grammar::BidirectionalStringMap::MAGIC_MARKER);
static constexpr int16_t ENDIANNESS_MARKER = 1;

static int8_t validateHeader(::inflection::util::MemoryMappedFile& mappedFile, const std::u16string& pathToFile)
{
    const char* magicMarker;
    mappedFile.read(&magicMarker, MAGIC_MARKER_LEN);
    if (strncmp(magicMarker, inflection::grammar::BidirectionalStringMap::MAGIC_MARKER, MAGIC_MARKER_LEN) != 0) {
        throw ::inflection::exception::IOException(u"Input file " + pathToFile + u" has an invalid header");
    }

    if (mappedFile.read<int64_t>() != inflection::grammar::BidirectionalStringMap::VERSION) {
        throw ::inflection::exception::IncompatibleVersionException(u"Input file " + pathToFile + u" has an incompatible version");
    }

    if (mappedFile.read<int16_t>() != ENDIANNESS_MARKER) {
        throw ::inflection::exception::IOException(u"Input file " + pathToFile + u" was built for a different architecture");
    }

    auto numberOfTries = mappedFile.read<int8_t>();
    if (numberOfTries != 1 && numberOfTries != 2) {
        throw ::inflection::exception::IOException(u"Unexpected number of tries");
    }

    return numberOfTries;
}

static uint8_t getOptions(::inflection::util::MemoryMappedFile &mappedFile)
{
    auto options = mappedFile.read<uint8_t>();

    const char* reserved;
    mappedFile.read(&reserved, inflection::grammar::BidirectionalStringMap::RESERVED_BYTES);

    return options;
}

static std::u16string getFromUnicodeSetPattern(::inflection::util::MemoryMappedFile &mappedFile) {
    auto patternSize = mappedFile.read<uint32_t>();

    const char* patternPtr;
    mappedFile.read(&patternPtr, patternSize);

    std::u16string pattern;
    std::u16string result;
    inflection::util::StringViewUtils::convert(&pattern, std::string_view(patternPtr, patternSize));
    inflection::util::StringViewUtils::replace(&result, pattern, std::u16string_view(u"\0", 1), u"");
    return result;
}

// TODO enable this test once the suffix files have been regenerated with the latest data.
TEST_CASE("ExemplarTest#testExemplarsInDictionary", "[.]")
{
    std::string srcDir = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path().parent_path().string();
    std::string resourcePath = srcDir + "/resources/org/unicode/inflection/exemplar/";

    auto files(TestUtils::listDirectoryContents(resourcePath));
    int32_t numFiles = 0;

    for (auto const& file : files) {
        auto srcFileStr(inflection::util::StringViewUtils::to_u16string(file));
        auto underscore = file.find('_');
        auto dot = file.find('.');
        if (underscore == std::string::npos || dot == std::string::npos) {
            FAIL("File can not parse the locale from " + file);
            return;
        }
        underscore++;
        std::string localeStr(file.substr(underscore, dot - underscore));
        inflection::util::ULocale locale(localeStr);
        const auto& dictionary = *npc(inflection::dictionary::DictionaryMetaData::createDictionary(locale));
        auto inflectionRoot = inflection::util::StringViewUtils::to_u16string(inflection::resources::DataRegistrationService::getDataPathForLocale(locale));
        if (inflectionRoot.empty()) {
#ifdef INFLECTION_ROOT
            inflectionRoot = inflection::util::StringViewUtils::to_u16string(INFLECTION_ROOT);
#endif
        }
        auto mapFileStr = inflectionRoot + inflection::util::StringViewUtils::to_u16string(INFLECTION_DATA_ROOT_DIR) + u"/inflection/exemplar/suffix_"
                    + ::inflection::util::StringViewUtils::to_u16string(localeStr) + u".bist";

        ::inflection::util::MemoryMappedFile mappedFile(mapFileStr);
        int8_t numberOfTries(validateHeader(mappedFile, mapFileStr));
        getOptions(mappedFile);
        ::icu4cxx::UnicodeSet sourceSet(getFromUnicodeSetPattern(mappedFile));
        ::icu4cxx::UnicodeSet targetSet(getFromUnicodeSetPattern(mappedFile));
        inflection::dictionary::metadata::MarisaTrie<int32_t> sourceTrie(&mappedFile);
        std::unique_ptr<inflection::dictionary::metadata::MarisaTrie<int32_t>> dynamicTrie(numberOfTries == 2 ? new inflection::dictionary::metadata::MarisaTrie<int32_t>(&mappedFile) : nullptr);
        const inflection::dictionary::metadata::MarisaTrie<int32_t>& targetTrie(dynamicTrie == nullptr ? sourceTrie : *npc(dynamicTrie.get()));

        for (const auto& [word, value] : targetTrie.getAllWithPrefix(u"")) {
            INFO(inflection::util::StringViewUtils::to_string(word));
            CHECK(dictionary.isKnownWord(word));
            CHECK(targetSet.containsAllCodePoints(word));
        }
        for (const auto& [suffix, value] : sourceTrie.getAllWithPrefix(u"")) {
            INFO(inflection::util::StringViewUtils::to_string(suffix));
            auto word = targetTrie.getKey(value);
            CHECK(word.ends_with(suffix));
            CHECK(sourceSet.containsAllCodePoints(suffix));
        }

        numFiles++;
    }
    REQUIRE(numFiles > 0);
}
