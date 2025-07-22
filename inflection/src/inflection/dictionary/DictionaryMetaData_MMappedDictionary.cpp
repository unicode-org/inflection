/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>

#include <inflection/dictionary/DictionaryKeyIterator.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/npc.hpp>
#include <string>
#include <memory>

namespace inflection::dictionary {

DictionaryMetaData_MMappedDictionary::DictionaryMetaData_MMappedDictionary(const inflection::util::ULocale& locale)
    : locale(locale)
{
}

DictionaryMetaData_MMappedDictionary::DictionaryMetaData_MMappedDictionary(::inflection::util::MemoryMappedFile* memoryMappedRegion, const ::std::u16string& sourcePath)
    : options(npc(memoryMappedRegion)->read<int16_t>())
    , locale(npc(memoryMappedRegion)->readArray<char>(MAX_LANGUAGE_CODE_LENGTH))
    , typesStringContainer(memoryMappedRegion)
    , bitsTypesSingletons(npc(memoryMappedRegion)->read<int8_t>())
    , bitsPropertyMapId(npc(memoryMappedRegion)->read<int8_t>())
    , bitsPropertyValueMapKey(npc(memoryMappedRegion)->read<int8_t>())
    , bitsPropertyValueMapValuesSize(npc(memoryMappedRegion)->read<int8_t>())
    , wordsToTypesSingletonsSize(npc(memoryMappedRegion)->read<int32_t>())
    , wordsToDataTrie(memoryMappedRegion)
    , wordsToTypesSingletons(npc(memoryMappedRegion)->readArray<int64_t>(wordsToTypesSingletonsSize))
    , wordsToDataSingletons(memoryMappedRegion)
    , propertyNameToKeyId(memoryMappedRegion)
    , propertyValuesStringContainer(memoryMappedRegion)
    , propertyValueMaps(memoryMappedRegion)
    , inflector((options & (int16_t)OptionBits::HAS_INFLECTION_TABLE) != 0 ? new Inflector(*npc(memoryMappedRegion), sourcePath, *this) : nullptr)
    , memoryMappedRegion(memoryMappedRegion)
    , inflectionKeyIdentifier(propertyNameToKeyId.getIdentifierIfAvailable(inflection::dictionary::Inflector_MMappedDictionary::INFLECTION_KEY))
    , bitsPropertyValueMapKeyMask((int32_t(1) << bitsPropertyValueMapKey) - 1)
    , bitsPropertyValueMapKeySize(inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(propertyNameToKeyId.size()))
    , isDoubleStageLookup(!wordsToDataSingletons.isEmpty())
{
}

DictionaryMetaData_MMappedDictionary::~DictionaryMetaData_MMappedDictionary()
{
}

const ::inflection::util::ULocale& DictionaryMetaData_MMappedDictionary::getLocale() const
{
    return locale;
}

std::optional<int64_t> DictionaryMetaData_MMappedDictionary::getWordType(std::u16string_view word) const
{
    auto result(wordsToDataTrie.find(word));
    if (result) {
        uint64_t dataSingleton = *result;
        if (isDoubleStageLookup) {
            dataSingleton = wordsToDataSingletons.read((int32_t) dataSingleton);
        }
        auto typeSingleton = inflection::dictionary::metadata::CompressedArray<int16_t>::extractValue(dataSingleton, 0, bitsTypesSingletons);
        if (typeSingleton != 0) {
            return {wordsToTypesSingletons[typeSingleton]};
        }
    }
    return {};
}

std::optional<int64_t> DictionaryMetaData_MMappedDictionary::getValueOfType(std::u16string_view type) const
{
    // An exception is thrown if the type is invalid.
    auto id = typesStringContainer.getIdentifierIfAvailable(type);
    if (id >= 0) {
        return inflection::dictionary::metadata::StringContainer::convertIdentifierToBit(id);
    }
    return {};
}

int64_t DictionaryMetaData_MMappedDictionary::getValuesOfTypes(const std::vector<std::u16string> &types) const
{
    int64_t result = 0;
    for (const auto& type : types) {
        auto value = getValueOfType(type);
        if (!value) {
            continue;
        }
        result |= *value;
    }
    return result;
}

std::optional<::std::u16string> DictionaryMetaData_MMappedDictionary::getTypeOfValue(int64_t value) const
{
    auto id = inflection::dictionary::metadata::StringContainer::convertBitToIdentifier(value);
    if (0 <= id && id < typesStringContainer.size()) {
        return typesStringContainer.getString(id);
    }
    return {};
}

std::vector<::std::u16string> DictionaryMetaData_MMappedDictionary::getTypesOfValues(int64_t value) const {
    ::std::vector<::std::u16string> result;
    while (value != 0) {
        const auto singleProperty = ((int64_t)(((uint64_t)(value ^ (value - 1))) >> 1)) + 1;
        value &= value - 1;
        const auto name = getTypeOfValue(singleProperty);
        if (name.has_value()) {
            result.push_back(name.value());
        }
    }
    return result;
}

DictionaryKeyIterator DictionaryMetaData_MMappedDictionary::getAllWords() const
{
    return DictionaryKeyIterator(wordsToDataTrie.getAllWithPrefix(u""));
}

int32_t DictionaryMetaData_MMappedDictionary::getAllWordsSize() const
{
    return wordsToDataTrie.getSize();
}

void DictionaryMetaData_MMappedDictionary::getPropertyMapInternalIdentifiers(std::vector<int32_t> &propertyIdentifiers, int32_t startingOffset, int32_t length) const
{
    propertyIdentifiers.reserve(length);
    for (int32_t idx = 0; idx < length; idx++) {
        propertyIdentifiers.emplace_back(propertyValueMaps.read(startingOffset + idx));
    }
}

bool DictionaryMetaData_MMappedDictionary::getWordPropertyInternalIdentifiers(std::vector<int32_t> &propertyIdentifiers, std::u16string_view word, int32_t propertyNameIdentifier) const {
    if (propertyNameIdentifier < 0) {
        return false;
    }

    auto trieResult(wordsToDataTrie.find(word));
    if (trieResult) {
        uint64_t dataSingleton = *trieResult;
        if (isDoubleStageLookup) {
            dataSingleton = wordsToDataSingletons.read((int32_t) dataSingleton);
        }
        auto offset = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(dataSingleton, bitsTypesSingletons, bitsPropertyMapId);
        if (offset != 0) {
            propertyIdentifiers.clear();

            const auto firstValue = propertyValueMaps.read(offset);
            int32_t cumulativeOffset = 0;
            int32_t originalMapSize = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(firstValue, bitsPropertyValueMapKey + bitsPropertyValueMapValuesSize, bitsPropertyValueMapKeySize);
            for (int32_t mapIdx = 0; mapIdx < originalMapSize; mapIdx++) {
                uint32_t keyEntry = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(propertyValueMaps.read(offset + mapIdx), 0, bitsPropertyValueMapKey + bitsPropertyValueMapValuesSize);
                int32_t key = keyEntry & bitsPropertyValueMapKeyMask;
                if (key > propertyNameIdentifier) {
                    // We missed the possible value. It's not going to get better.
                    break;
                }
                int32_t valuesSegmentSize = keyEntry >> bitsPropertyValueMapKey;
                if (key == propertyNameIdentifier) {
                    getPropertyMapInternalIdentifiers(propertyIdentifiers, offset + originalMapSize + cumulativeOffset, valuesSegmentSize);
                    return true;
                }
                cumulativeOffset += valuesSegmentSize;
            }
        }
    }
    return false;
}

bool DictionaryMetaData_MMappedDictionary::getWordPropertyValues(::std::vector<::std::u16string>* result, std::u16string_view word, std::u16string_view property) const
{
    const auto propertyNameIdentifier = propertyNameToKeyId.getIdentifier(property);
    if (propertyNameIdentifier < 0) {
        // Invalid property supplied
        return false;
    }

    std::vector<int32_t> propertyIdentifiers;
    if (!getWordPropertyInternalIdentifiers(propertyIdentifiers, word, propertyNameIdentifier)) {
        // Getting identifiers operation failed, return false
        return false;
    }

    if (result == nullptr) {
        // If result vector is null do nothing
        return true;
    }

    const bool useInflectionTrieForKeys = (inflector.get() != nullptr) && (propertyNameIdentifier == inflectionKeyIdentifier);

    for (const auto propertyIdentifier: propertyIdentifiers) {
        npc(result)->emplace_back(useInflectionTrieForKeys
                ? inflector->mmappedDictionary.identifierToInflectionPatternTrie.getKey(propertyIdentifier)
                : propertyValuesStringContainer.getString(propertyIdentifier));
    }

    return true;
}

DictionaryMetaData_MMappedDictionary* DictionaryMetaData_MMappedDictionary::createDictionary(const ::std::u16string& sourcePath)
{
    // RAII exception handling
    ::std::unique_ptr<::inflection::util::MemoryMappedFile> mappedFile(new ::inflection::util::MemoryMappedFile(sourcePath));
    auto mappedRegion = mappedFile.get();

    if (mappedRegion->getSize() < sizeof(MAGIC_MARKER) + sizeof(VERSION) + sizeof(ENDIANNESS_MARKER)) {
        throw ::inflection::exception::IOException( u"Input file " + sourcePath + u" is too small");
    }

    // Verify header starts with magic token
    const char* magicMarker = npc(mappedRegion)->readArray<char>(sizeof(MAGIC_MARKER));
    if (strncmp(magicMarker, MAGIC_MARKER, sizeof(MAGIC_MARKER)) != 0) {
        throw ::inflection::exception::IOException(u"Input file " + sourcePath + u" has invalid header");
    }

    auto version = npc(mappedRegion)->read<int64_t>();
    if (version != VERSION) {
        throw ::inflection::exception::IncompatibleVersionException(u"Input file " + sourcePath + u" has an incompatible version");
    }

    auto endiannessMarker = npc(mappedRegion)->read<int16_t>();
    if (endiannessMarker != ENDIANNESS_MARKER) {
        throw ::inflection::exception::IOException(u"Input file " + sourcePath + u" was built for a different architecture");
    }

    auto dictionaryMetaData = new DictionaryMetaData_MMappedDictionary(mappedRegion, sourcePath);

    // Release because dictionaryMetaData now owns this pointer.
    mappedFile.release();
    return dictionaryMetaData;
}

} // namespace inflection::dictionary
