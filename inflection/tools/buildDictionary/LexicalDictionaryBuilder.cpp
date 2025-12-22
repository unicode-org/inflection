/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
// This is the writable portion of DictionaryMetaData_MMappedDictionary.
#include "LexicalDictionaryBuilder.hpp"

#include "InflectionDictionary.hpp"
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/dictionary/metadata/StringArrayContainer.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/npc.hpp>

#include <string>
#include <memory>
#include <fstream>

using inflection::dictionary::DictionaryMetaData_MMappedDictionary;
using inflection::dictionary::metadata::CompressedArray;
using inflection::dictionary::metadata::MarisaTrie;
using inflection::dictionary::metadata::StringArrayContainer;
using inflection::dictionary::metadata::StringContainer;

template <typename T>
inline void LexicalDictionaryBuilder::writeValue(uint64_t &valueBase, int32_t start, int32_t len, T valueToWrite) {
    if (!(0 <= start && (start+len) < 64)) {
        throw inflection::exception::IndexOutOfBoundsException(u"Illegal start or length value");
    }
    auto unsignedValueWrite = (uint64_t) valueToWrite;
    unsignedValueWrite &= ((uint64_t(1) << len) - 1);
    valueBase |= (unsignedValueWrite << start);
}

void LexicalDictionaryBuilder::write(::std::ofstream& writer,
                                     DictionaryLogger& logger,
                                     const ::inflection::util::ULocale& locale,
                                     int8_t bitsTypesSingletons,
                                     int8_t bitsPropertyMapId,
                                     int8_t bitsPropertyMapKeyId,
                                     int8_t bitsPropertyMapValuesSize,
                                     const inflection::dictionary::metadata::MarisaTrie<uint64_t>& wordsToDataTrie,
                                     const std::vector<int64_t>& typeSingletons,
                                     const inflection::dictionary::metadata::CompressedArray<uint64_t>& wordsToDataSingletons,
                                     const inflection::dictionary::metadata::StringArrayContainer& propertyNameToKeyId,
                                     const inflection::dictionary::metadata::StringContainer& propertyValuesStringContainer,
                                     const inflection::dictionary::metadata::CompressedArray<int32_t>& propertyValueMaps,
                                     const inflection::dictionary::metadata::StringArrayContainer& typesStringContainer,
                                     bool hasInflectionTable)
{
    writer.write(DictionaryMetaData_MMappedDictionary::MAGIC_MARKER, sizeof(DictionaryMetaData_MMappedDictionary::MAGIC_MARKER));
    writer.write(reinterpret_cast<const char*>(&DictionaryMetaData_MMappedDictionary::VERSION), sizeof(DictionaryMetaData_MMappedDictionary::VERSION));
    writer.write(reinterpret_cast<const char*>(&DictionaryMetaData_MMappedDictionary::ENDIANNESS_MARKER), sizeof(DictionaryMetaData_MMappedDictionary::ENDIANNESS_MARKER));
    // Write structure options. Currently unused.
    int16_t options = 0;
    if (hasInflectionTable) {
        options |= int16_t(DictionaryMetaData_MMappedDictionary::OptionBits::HAS_INFLECTION_TABLE);
    }
    writer.write(reinterpret_cast<const char*>(&options), sizeof(DictionaryMetaData_MMappedDictionary::OPTIONS));

    const auto& language = locale.getLanguage();
    if (language.length() >= DictionaryMetaData_MMappedDictionary::MAX_LANGUAGE_CODE_LENGTH) {
        throw ::inflection::exception::IOException(u"The language code of this metadata is too long for serialization.");
    }
    writer.write(language.data(), language.length());
    for (int32_t i = int32_t(language.length()); i < DictionaryMetaData_MMappedDictionary::MAX_LANGUAGE_CODE_LENGTH; ++i) {
        writer.write("\0", sizeof(char));
    }
    logger.logWithOffset(locale.getName() + " header");

    typesStringContainer.write(writer);
    logger.logWithOffset(locale.getName() + " typesStringContainer");
    writer.write(reinterpret_cast<const char*>(&bitsTypesSingletons), sizeof(bitsTypesSingletons));
    logger.logWithOffset(locale.getName() + " bitsTypesSingletons");
    writer.write(reinterpret_cast<const char*>(&bitsPropertyMapId), sizeof(bitsPropertyMapId));
    logger.logWithOffset(locale.getName() + " bitsPropertyMapId");
    writer.write(reinterpret_cast<const char*>(&bitsPropertyMapKeyId), sizeof(bitsPropertyMapKeyId));
    logger.logWithOffset(locale.getName() + " bitsPropertyMapKeyId");
    writer.write(reinterpret_cast<const char*>(&bitsPropertyMapValuesSize), sizeof(bitsPropertyMapValuesSize));
    logger.logWithOffset(locale.getName() + " bitsPropertyMapValuesSize");

    int32_t wordsToTypesSingletonsSize = int32_t(typeSingletons.size());
    writer.write(reinterpret_cast<const char*>(&wordsToTypesSingletonsSize), sizeof(wordsToTypesSingletonsSize));
    logger.logWithOffset(locale.getName() + " wordsToTypesSingletonsSize");
    wordsToDataTrie.write(writer);
    logger.logWithOffset(locale.getName() + " wordsToDataTrie");
    writer.write(reinterpret_cast<const char*>(typeSingletons.data()), sizeof(typeSingletons[0]) * wordsToTypesSingletonsSize);
    logger.logWithOffset(locale.getName() + " wordsToTypesSingletons");

    wordsToDataSingletons.serialize(writer);
    logger.logWithOffset(locale.getName() + " wordsToDataSingletons");

    propertyNameToKeyId.write(writer);
    propertyValuesStringContainer.write(writer);

    propertyValueMaps.serialize(writer);
    logger.logWithOffset(locale.getName() + " wordToPropertyValueMap");
}

static ::std::map<int64_t, int64_t> createBitsToContainerBitsMap(
        const std::map<::std::u16string_view, int32_t>& typeStringIndices,
        const ::std::map<int64_t, ::std::u16string_view>& valueToType)
{
    std::map<int64_t, int64_t> valueBitsToContainerBits;
    for (const auto& [value, type] : valueToType) {
        auto indexItr = typeStringIndices.find(type);
        if (indexItr == typeStringIndices.end()) {
            throw inflection::exception::IllegalArgumentException(u"string not found in typeStringIndices");
        }
        auto identifierBit = StringContainer::convertIdentifierToBit(indexItr->second);
        if (inflection::dictionary::metadata::StringContainer::convertBitToIdentifier(identifierBit) != indexItr->second) {
            throw inflection::exception::IllegalArgumentException(u"bit did not roundtrip");
        }

        valueBitsToContainerBits.emplace(value, identifierBit);
    }
    return valueBitsToContainerBits;
}

static int64_t mapValueToContainerBits(int64_t binaryProperties, std::map<int64_t, int64_t> bitmap)
{
    int64_t result = 0;
    while (binaryProperties != 0) {
        auto singleProperty = ((int64_t) (((uint64_t) (binaryProperties ^ (binaryProperties - 1))) >> 1)) + 1;
        binaryProperties &= binaryProperties - 1;
        auto newBitItr = bitmap.find(singleProperty);
        if (newBitItr == bitmap.end()) {
            throw inflection::exception::IllegalArgumentException(u"bit not found in bitmap");
        }
        result |= newBitItr->second;
    }
    return result;
}

static void
createPropertyValueMap(
        const std::map<::std::u16string_view, ::std::map<::std::u16string_view, ::std::vector<std::u16string_view>>>& data,
        InflectionDictionary* inflectionDictionary,
        int8_t& bitsPropertyMapKeyId,
        int8_t& bitsPropertyMapValuesSize,
        std::map<::std::u16string_view, int32_t>& wordToPropertyValueMapIdMap,
        StringArrayContainer*& propertyNameToKeyId,
        StringContainer*& propertyValuesStringContainer,
        std::vector<int32_t>& propertyValueMaps)
{
    ::std::map<::std::u16string_view, int32_t> keyContainerMap;
    ::std::map<::std::u16string_view, int32_t> valueContainerMap;
    ::std::map<::std::u16string_view, int32_t> inflectionContainerMap;

    // Get statistics for the map.
    ::std::set<::std::u16string_view> knownKeys;
    ::std::set<::std::u16string_view> knownValues;
    int32_t expectedLength = 0;
    int32_t valuesSizeMask = 0;
    for (const auto& [word, maps] : data) {
        expectedLength += maps.size();
        for (const auto& [key, values] : maps) {
            auto valuesSize = values.size();
            expectedLength += valuesSize;
            valuesSizeMask |= valuesSize;

            knownKeys.insert(key);
            if (inflectionDictionary == nullptr || key != ::inflection::dictionary::Inflector_MMappedDictionary::INFLECTION_KEY) {
                knownValues.insert(values.begin(), values.end());
            }
            else {
                for (const auto& value : values) {
                    auto inflectionVal = inflectionContainerMap.find(value);
                    if (inflectionVal == inflectionContainerMap.end()) {
                        auto numID = npc(inflectionDictionary)->getId(value);
                        if (numID == -1) {
                            throw inflection::exception::IllegalStateException(std::u16string(value) + u" inflection name not found.");
                        }
                        inflectionContainerMap.emplace(value, numID);
                    }
                }
            }
        }
    }

    propertyNameToKeyId = new StringArrayContainer(knownKeys, &keyContainerMap);
    propertyValuesStringContainer = new StringContainer(knownValues, &valueContainerMap);
    propertyValueMaps.reserve(expectedLength);
    propertyValueMaps.emplace_back(0); // Placeholder for invalid value.
    if (keyContainerMap.empty()) {
        // nothing to do. Use empty values.
        return;
    }

    // Check sizes of fields.
    bitsPropertyMapKeyId = CompressedArray<int32_t>::calculateBitWidth(keyContainerMap.size() - 1); // zero based
    bitsPropertyMapValuesSize = CompressedArray<int32_t>::calculateBitWidth(valuesSizeMask);
    int8_t bitsKeysSize = CompressedArray<int32_t>::calculateBitWidth(keyContainerMap.size());
    if ((bitsKeysSize + bitsPropertyMapValuesSize + bitsPropertyMapKeyId) > 32) {
        throw ::inflection::exception::IllegalStateException(u"Too many property keys");
    }

    // Writing four characters into the stringstream so that the offsets start from a non-zero value
    ::std::map<std::vector<int32_t>, int32_t> mapIdSingletons;
    ::std::vector<std::vector<int32_t>> vectorBookkeeping;
    ::std::vector<int32_t> vectorData;

    vectorBookkeeping.resize(keyContainerMap.size()); // The keyID is the index, and then all the values in key order.
    uint32_t inflectionKeyId = UINT32_MAX; // Initialize to invalid value. We aren't going to have 4 billion keys.
    auto inflectionKeyIdIter(keyContainerMap.find(inflection::dictionary::Inflector_MMappedDictionary::INFLECTION_KEY));
    if (!inflectionContainerMap.empty() && inflectionKeyIdIter != keyContainerMap.end()) {
        // We have an inflection table AND we have inflection key usage.
        inflectionKeyId = inflectionKeyIdIter->second;
    }
    // else we're not looking up the values in inflectionContainerMap.

    /**
     * We store every string in every sub-array in a single memory block, one after another.
     * The lookup then returns an offset in an array. The first field has the keyID, number of values and the number of keys.
     * When there is more than one key, there is only the keyID and the size of the array of values for that key after the first key slot.
     * The keys are in ascending order.
     */
    for (const auto& [word, wordEntry] : data) {
        for (const auto& [propertyName, properties] : wordEntry) {
            auto propertyKeyIdIter(keyContainerMap.find(propertyName));
            if (propertyKeyIdIter == keyContainerMap.end()) {
                throw ::inflection::exception::IllegalStateException(u"Key not found: " + std::u16string(propertyName));
            }
            uint32_t propertyKeyId = propertyKeyIdIter->second;
            for (const auto& property : properties) {
                const auto &containerLookupMap = (propertyKeyId == inflectionKeyId) ? inflectionContainerMap : valueContainerMap;
                auto strId = containerLookupMap.find(property);
                if (strId == containerLookupMap.end()) {
                    throw ::inflection::exception::IllegalStateException(u"PropertyValueMap: Could not find string " + ::std::u16string(property) + u" in string container.");
                }
                vectorBookkeeping[propertyKeyId].emplace_back(strId->second);
            }
        }

        // We're setting all the used key identifiers with their sizes.
        vectorData.clear();
        int32_t keyID = 0;
        for (const auto& values : vectorBookkeeping) {
            if (!values.empty()) {
                int32_t value = keyID | int32_t(values.size() << bitsPropertyMapKeyId);
                vectorData.emplace_back(value);
            }
            keyID++;
        }
        if (vectorData.empty()) {
            throw ::inflection::exception::IllegalStateException(u"PropertyValueMap: Map for " + ::std::u16string(word) + u" can not be empty.");
        }

        // The initial value also has the number of keys.
        vectorData[0] |= int32_t(vectorData.size() << (bitsPropertyMapKeyId + bitsPropertyMapValuesSize));
        // Append all the vectors in key order
        for (auto& values : vectorBookkeeping) {
            if (!values.empty()) {
                vectorData.insert(vectorData.end(), values.begin(), values.end());
                values.clear(); // We don't need the data anymore.
            }
        }

        auto [singleton, inserted](mapIdSingletons.emplace(vectorData, (int32_t)propertyValueMaps.size()));
        if (inserted) {
            // Normal addition
            propertyValueMaps.insert(propertyValueMaps.end(), vectorData.begin(), vectorData.end());
        }
        // else reuse existing value.
        wordToPropertyValueMapIdMap.emplace(word, singleton->second);
    }
}


void LexicalDictionaryBuilder::writeDictionary(::std::ofstream& writer,
                                               DictionaryLogger& logger,
                                               const Dictionary &dictionary,
                                               const ::std::string& sourceInflectionFilename,
                                               bool addAffixPatternMappings)
{
    ::std::set<::std::u16string_view> typeStrings;
    for (auto [bits, name] : dictionary.getType()) {
        typeStrings.insert(name);
    }

    ::std::map<::std::u16string_view, int32_t> typeStringIndices;
    StringArrayContainer stringContainer(typeStrings, &typeStringIndices);

    bool hasInflectionTable = !sourceInflectionFilename.empty();
    InflectionDictionary* inflectionDictionary = nullptr;
    if (hasInflectionTable) {
        Inflector_InflectionDictionary* parsedDictionary = InflectionDictionary::readXML(dictionary.getLocale(), sourceInflectionFilename, typeStringIndices);
        inflectionDictionary = new InflectionDictionary(dictionary.getLocale(), *parsedDictionary, addAffixPatternMappings);
        delete parsedDictionary;
    }

    // Compress the data structure
    ::std::map<::std::u16string_view, int16_t> wordsToTypesSingletons;
    ::std::map<int64_t, int16_t> typeSingletonsMapToIndex;
    ::std::vector<int64_t> typeSingletons;
    std::map<int64_t, int64_t> valueBitsToContainerBitsMap(createBitsToContainerBitsMap(typeStringIndices, dictionary.getValueToType()));
    int64_t usedProperties = 0;

    //Adding a dummy element to enforce the mapping to start from 1
    typeSingletons.emplace_back((int64_t) 0);
    for (const auto& [word, properties] : dictionary.getWordsToTypes()) {
        usedProperties |= properties;
        auto previousIndex = typeSingletonsMapToIndex.find(properties);
        if (previousIndex == typeSingletonsMapToIndex.end()) {
            typeSingletonsMapToIndex.emplace(properties, (int16_t)typeSingletons.size());
            wordsToTypesSingletons.emplace(word, (int16_t)typeSingletons.size());
            typeSingletons.emplace_back(mapValueToContainerBits(properties, valueBitsToContainerBitsMap));
        }
        else {
            wordsToTypesSingletons.emplace(word, previousIndex->second);
        }
    }
    if (typeSingletons.size() >= INT16_MAX) {
        throw inflection::exception::IllegalArgumentException(u"Too many type singletons");
    }
    if (logger.isVerbose() && !typeStrings.empty()) {
        for (uint64_t bit = (uint64_t(1) << (typeStrings.size() - 1)); bit; bit >>= 1) {
            if ((bit & usedProperties) == 0) {
                logger.log(dictionary.getLocale().getName() + " " + inflection::util::StringViewUtils::to_string(dictionary.getValueToType().find(bit)->second) + " is unreferenced in the dictionary");
            }
        }
    }

    ::std::map<::std::u16string_view, int32_t> wordToPropertyMapId;
    StringArrayContainer* propertyNameToKeyId = nullptr;
    StringContainer* propertyValuesStringContainer = nullptr;
    std::vector<int32_t> propertyValueMapsVector;
    int8_t bitsPropertyMapKeyId = 0;
    int8_t bitsPropertyMapValuesSize = 0;
    createPropertyValueMap(dictionary.getWordToPropertyValue(),
                           inflectionDictionary,
                           bitsPropertyMapKeyId,
                           bitsPropertyMapValuesSize,
                           wordToPropertyMapId,
                           propertyNameToKeyId,
                           propertyValuesStringContainer,
                           propertyValueMapsVector);
    inflection::dictionary::metadata::CompressedArray propertyValueMaps(propertyValueMapsVector);

    auto bitsTypesSingletons = getNumBitsFromValues(wordsToTypesSingletons);
    auto bitsPropertyMapId = getNumBitsFromValues(wordToPropertyMapId);
    ::std::map<::std::u16string_view, uint64_t> wordsToData;
    for (const auto& [word, type] : wordsToTypesSingletons) {
        writeValue(wordsToData[word], 0, bitsTypesSingletons, type);
    }
    for (const auto& [word, property] : wordToPropertyMapId) {
        writeValue(wordsToData[word], bitsTypesSingletons, bitsPropertyMapId, property);
    }

    // Compress the data structure
    ::std::map<::std::u16string_view, uint64_t> wordsToDataSingletons;
    ::std::map<uint64_t, uint64_t> dataSingletonsMapToIndex;
    ::std::vector<uint64_t> dataSingletons;
    uint64_t wordsToDataTrieBitMask = 0;
    for (auto [word, data] : wordsToData) {
        auto [index, inserted](dataSingletonsMapToIndex.emplace(data, (uint64_t)dataSingletons.size()));
        if (inserted) {
            dataSingletons.emplace_back(data);
        }
        wordsToDataSingletons.emplace(word, index->second);
        wordsToDataTrieBitMask |= data;
    }

    // Estimate which data configuration will be smaller
    int8_t numBitsWordsToDataTrieArray = inflection::dictionary::metadata::CompressedArray<int64_t>::calculateBitWidth(wordsToDataTrieBitMask);
    int8_t numBitsDataSingletonsMapToIndexArray = inflection::dictionary::metadata::CompressedArray<int64_t>::calculateBitWidth(dataSingletons.size());
    int64_t estimated1Stage = wordsToData.size() * int64_t(numBitsWordsToDataTrieArray);
    int64_t estimated2Stage = dataSingletons.size() * int64_t(numBitsWordsToDataTrieArray) + wordsToData.size() * numBitsDataSingletonsMapToIndexArray;
    bool use2Stage = estimated1Stage > estimated2Stage;
    if (logger.isVerbose()) {
        logger.log(dictionary.getLocale().getName() + " use2Stage=" + (use2Stage ? "true" : "false") + " " + std::to_string((estimated1Stage - estimated2Stage)/8) + " estimated byte difference");
    }
    if (!use2Stage) {
        // It will be smaller as a 2 stage lookup instead of a single stage lookup.
        dataSingletons.clear();
    }

    CompressedArray dataSingletonsRaw(dataSingletons);

    MarisaTrie wordsToDataTrie(use2Stage ? wordsToDataSingletons : wordsToData);

    write(writer,
          logger,
          dictionary.getLocale(),
          bitsTypesSingletons,
          bitsPropertyMapId,
          bitsPropertyMapKeyId,
          bitsPropertyMapValuesSize,
          wordsToDataTrie,
          typeSingletons,
          dataSingletonsRaw,
          *npc(propertyNameToKeyId),
          *npc(propertyValuesStringContainer),
          propertyValueMaps,
          stringContainer,
          hasInflectionTable);

    if (inflectionDictionary != nullptr) {
        inflectionDictionary->write(writer, logger);
        delete inflectionDictionary;
    }

    delete propertyNameToKeyId;
    delete propertyValuesStringContainer;
}
