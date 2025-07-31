/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::dictionary {

static const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary& verifyMemoryMappedFileHeader(::inflection::util::MemoryMappedFile& mappedFile, const ::std::u16string& sourcePath, const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary& dictionary) {
    int64_t version;
    mappedFile.read(&version);
    if (version != Inflector_MMappedDictionary::VERSION) {
        throw ::inflection::exception::IncompatibleVersionException(u"Inflection file " + sourcePath + u" has an incompatible version "
            + ::inflection::util::StringUtils::to_u16string(version) + u" and expected version " + ::inflection::util::StringUtils::to_u16string(Inflector_MMappedDictionary::VERSION));
    }

    int16_t endiannessMarker;
    mappedFile.read(&endiannessMarker);
    if (endiannessMarker != Inflector_MMappedDictionary::ENDIANNESS_MARKER) {
        throw ::inflection::exception::IOException(u"Inflection file " + sourcePath + u" was built for a different architecture");
    }

    // Read options. Currently unused.
    mappedFile.readArray<char>(sizeof(Inflector_MMappedDictionary::OPTIONS));

    return dictionary;
}

Inflector_MMappedDictionary::Inflector_MMappedDictionary(inflection::util::MemoryMappedFile& memoryMappedFile, const std::u16string &sourcePath, const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary &dictionary)
    : locale(verifyMemoryMappedFileHeader(memoryMappedFile, sourcePath, dictionary).getLocale())
    , grammemePatternsSize(memoryMappedFile.read<int32_t>())
    , grammemePatterns(memoryMappedFile.readArray<int64_t>(grammemePatternsSize))
    , inflectionSuffixes(&memoryMappedFile)
    , dictionary(dictionary)
    , inflectionsArray(&memoryMappedFile)
    , numBitsForGrammemesIdx(memoryMappedFile.read<int8_t>())
    , numBitsForSuffixIdx(memoryMappedFile.read<int8_t>())
    , numBitsForSuffixToIdentifierRunsIdx(memoryMappedFile.read<int8_t>())
    , numBitsForSuffixToIdentifierRunsLen(memoryMappedFile.read<int8_t>())
    , frequencyArraySize(memoryMappedFile.read<int32_t>())
    , frequenciesArray(memoryMappedFile.readArray<int32_t>(frequencyArraySize))
    , identifierToInflectionPatternTrie(&memoryMappedFile)
    , suffixToIdentifierRunsTrie(&memoryMappedFile)
    , suffixesToInflectionsIDArray(&memoryMappedFile)
    , numBitsForFrequencyIdx(memoryMappedFile.read<int8_t>())
    , numBitsForPOSIdx(memoryMappedFile.read<int8_t>())
    , numBitsForLemmaSuffixesLen(memoryMappedFile.read<int8_t>())
    , numBitsForNumOfInflections(memoryMappedFile.read<int8_t>())
    , numBitsForPrefixIdx(memoryMappedFile.read<int8_t>())
    , hasSuffixToIdentifiersMapping(!suffixesToInflectionsIDArray.isEmpty())
{
}

Inflector_MMappedDictionary::~Inflector_MMappedDictionary()
{
}

std::optional<int16_t> Inflector_MMappedDictionary::getInflectionPatternIndexFromName(std::u16string_view name) const {
    return identifierToInflectionPatternTrie.getKeyId(name);
}

Inflector_InflectionPattern Inflector_MMappedDictionary::getInflectionPattern(int32_t index) const {
    auto patternIndex = identifierToInflectionPatternTrie.find(index);
    uint64_t inflectionPatternPrefix = inflectionsArray.read(patternIndex);
    int16_t numOfInflections = inflection::dictionary::metadata::CompressedArray<int16_t>::extractValue(inflectionPatternPrefix, 0, numBitsForNumOfInflections);
    inflectionPatternPrefix >>= numBitsForNumOfInflections;

    int16_t lemmaSuffixesLen = inflection::dictionary::metadata::CompressedArray<int16_t>::extractValue(inflectionPatternPrefix, 0, numBitsForLemmaSuffixesLen);
    inflectionPatternPrefix >>= numBitsForLemmaSuffixesLen;

    int32_t posIndex = inflection::dictionary::metadata::CompressedArray<int16_t>::extractValue(inflectionPatternPrefix, 0, numBitsForPOSIdx);
    inflectionPatternPrefix >>= numBitsForPOSIdx;

    int32_t frequencyIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(inflectionPatternPrefix, 0, numBitsForFrequencyIdx);

    int64_t partsOfSpeech = grammemePatterns[posIndex];

    // resolve inflection pattern data
    return Inflector_InflectionPattern(
            index,
            frequencyIdx,
            partsOfSpeech,
            lemmaSuffixesLen,
            patternIndex + 1,
            numOfInflections,
            patternIndex + 1 + lemmaSuffixesLen,
            *this
    );
}

bool Inflector_MMappedDictionary::getInflectionPatternIdentifiers(std::vector<int32_t>& inflectionIdentifiers, std::u16string_view word) const {
    auto exists = dictionary.getWordPropertyInternalIdentifiers(inflectionIdentifiers, word, dictionary.inflectionKeyIdentifier) != DictionaryMetaData_MMappedDictionary::UNKNOWN;
    if (!exists && !inflection::util::StringViewUtils::isAllLowerCase(word)) {
        ::std::u16string normalized;
        DictionaryMetaData::transform(&normalized, word, locale);
        if (normalized != word) {
            exists = dictionary.getWordPropertyInternalIdentifiers(inflectionIdentifiers, normalized, dictionary.inflectionKeyIdentifier) != DictionaryMetaData_MMappedDictionary::UNKNOWN;
        }
    }
    return exists;
}

std::vector<Inflector_InflectionPattern> Inflector_MMappedDictionary::getInflectionPatternsFromSuffix(std::u16string_view suffix) const {
    inflection::util::Validate::isTrue(hasSuffixToIdentifiersMapping, u"Inflection dictionary is missing suffix to identifiers mappings");
    auto searchResult(suffixToIdentifierRunsTrie.find(suffix));
    if (!searchResult) {
        return {};
    }
    auto suffixPatternVal = uint32_t(*searchResult);

    // resolve suffixes offsets and counts
    auto suffixesOffset = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(suffixPatternVal, numBitsForSuffixToIdentifierRunsLen, numBitsForSuffixToIdentifierRunsIdx);
    auto suffixCount = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(suffixPatternVal, 0, numBitsForSuffixToIdentifierRunsLen);

    // resolve inflection identifiers for suffix
    std::vector<Inflector_InflectionPattern> result;
    result.reserve(suffixCount);
    for (int16_t i = 0; i < suffixCount; ++i) {
        auto id = suffixesToInflectionsIDArray.read(suffixesOffset + i);
        result.emplace_back(getInflectionPattern(id));
    }

    return result;
}
} // namespace inflection::dictionary
