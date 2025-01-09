/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#include "InflectionDictionary.hpp"
#include "LexicalDictionaryBuilder.hpp"
#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/util/Finally.hpp>
#include <inflection/npc.hpp>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include <algorithm>
#include <assert.h>
#include <set>
#include <string>
#include <map>
#include <vector>
#include <fstream>

InflectionDictionary::Inflection::~Inflection() {
}

InflectionDictionary::InflectionPattern::~InflectionPattern() {
}

int8_t calculateBitWidth(uint64_t allUsedBits)
{
    int8_t wordWidth = 0;
    while (allUsedBits != 0) {
        wordWidth++;
        allUsedBits >>= 1;
    }
    if (wordWidth == 0) {
        wordWidth = 1;
    }
    return wordWidth;
}


static int32_t getValue(const std::map<::std::u16string_view, int32_t>& map, ::std::u16string_view key)
{
    auto itr = map.find(key);
    if (itr == map.end()) {
        throw inflection::exception::IllegalArgumentException(std::u16string(key) + u" key not found");
    }
    return itr->second;
}

int32_t InflectionDictionary::getGrammemeSingletonIndex(std::map<int64_t, int32_t>& grammemesSingletonsMap, int64_t grammemes)
{
    int32_t grammemesIdx = 0;
    auto grammemeItr = grammemesSingletonsMap.find(grammemes);
    if (grammemeItr == grammemesSingletonsMap.end()) {
        // add grammemes
        grammemesIdx = int32_t(grammemePatterns.size());
        grammemesSingletonsMap.emplace(grammemes, grammemesIdx);
        grammemePatterns.emplace_back(grammemes);
    }
    else {
        grammemesIdx = grammemeItr->second;
    }
    return grammemesIdx;
}

InflectionDictionary::InflectionDictionary(const ::inflection::util::ULocale &locale, const Inflector_InflectionDictionary &dictionary, bool addAffixPatternMappings)
    : locale(locale)
{
    std::map<std::u16string_view, int32_t> mappingResult;

    // INFLECTION PATTERNS
    // ------------------------------------- convert Inflection_InflectionPatterns to POD -----------------------------------
    std::map<::std::u16string_view, int32_t> identifierToInflectionPatternMap;
    std::set<::std::u16string_view> inflection_SuffixesSet;
    std::map<int64_t, int32_t> grammemesSingletonsMap;
    std::set<int32_t> frequenciesSet;
    std::map<int32_t, int32_t> frequenciesMap;
    int32_t inflectionsTotalSize = 0;

    int32_t usedBitsForLemmaSuffixesLen = 0;
    int32_t usedBitsForNumOfInflections = 0;

    // Collect data for compression
    for (const auto& pattern : dictionary.inflectionPatterns) {
        frequenciesSet.insert(pattern.frequency);
        // Save the POS grammemes
        getGrammemeSingletonIndex(grammemesSingletonsMap, pattern.partsOfSpeech);
        for (const auto & lemmaSuffix : pattern.lemmaSuffixes) {
            inflection_SuffixesSet.insert(lemmaSuffix);
        }
        for (const auto & inflectionForGrammeme : pattern.inflectionForGrammeme) {
            inflection_SuffixesSet.insert(inflectionForGrammeme.suffix);
        }
        inflectionsTotalSize += pattern.inflectionForGrammeme.size();
        usedBitsForLemmaSuffixesLen |= pattern.lemmaSuffixes.size();
        usedBitsForNumOfInflections |= pattern.inflectionForGrammeme.size();
    }

    numBitsForFrequencyIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(frequenciesSet.size());
    numBitsForPOSIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(grammemesSingletonsMap.size());
    numBitsForLemmaSuffixesLen = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(usedBitsForLemmaSuffixesLen);
    numBitsForNumOfInflections = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(usedBitsForNumOfInflections);

    frequencyArray.reserve(frequenciesSet.size());
    int32_t frequenciesCount = 0;
    for (auto frequency : frequenciesSet) {
        frequenciesMap.emplace(frequency, frequenciesCount);
        frequencyArray.emplace_back(frequency);
        frequenciesCount++;
    }

    inflectionsArray.reserve(inflectionsTotalSize + inflection_SuffixesSet.size());

    std::map<::std::u16string_view, int32_t> inflection_SuffixesMap;
    inflection_Suffixes = new inflection::dictionary::metadata::StringContainer(inflection_SuffixesSet, &inflection_SuffixesMap);

    std::vector<int64_t> currentInflections;

    for (const auto& pattern : dictionary.inflectionPatterns) {
        for (const auto& inflection : pattern.inflectionForGrammeme) {
            getGrammemeSingletonIndex(grammemesSingletonsMap, inflection.grammemes);
        }
    }
    numBitsForGrammemesIdx = LexicalDictionaryBuilder::getNumBitsFromValues(grammemesSingletonsMap);

    // Fill out the rest of the grammemesSingletonsMap. The POS grammemes are first to reduce the number of bits used.
    for (const auto& pattern : dictionary.inflectionPatterns) {
        identifierToInflectionPatternMap.emplace(pattern.identifier, inflectionsArray.size());

        int32_t posIdx = getGrammemeSingletonIndex(grammemesSingletonsMap, pattern.partsOfSpeech);
        int64_t patternPrefix = frequenciesMap.find(pattern.frequency)->second;
        patternPrefix = (patternPrefix << numBitsForPOSIdx) | posIdx;
        patternPrefix = (patternPrefix << numBitsForLemmaSuffixesLen) | pattern.lemmaSuffixes.size();
        patternPrefix = (patternPrefix << numBitsForNumOfInflections) | pattern.inflectionForGrammeme.size();
        inflectionsArray.emplace_back(patternPrefix);

        for (const auto& lemmaSuffix : pattern.lemmaSuffixes) {
            inflectionsArray.emplace_back(inflection_SuffixesMap[lemmaSuffix]);
        }
        for (const auto& inflection : pattern.inflectionForGrammeme) {
            int32_t inflectionGrammemesIdx = getGrammemeSingletonIndex(grammemesSingletonsMap, inflection.grammemes);
            int64_t bits = (inflection_SuffixesMap[inflection.suffix] << numBitsForGrammemesIdx) | inflectionGrammemesIdx;
            inflectionsArray.emplace_back(bits);
        }
    }
    numBitsForSuffixIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(inflection_SuffixesMap.size());

    if (numBitsForGrammemesIdx + numBitsForSuffixIdx > 64) {
        throw inflection::exception::IllegalStateException(u"Too many bits used for inflections.");
    }

    identifierToInflectionPatternTrie = new inflection::dictionary::metadata::MarisaTrie<int32_t>(identifierToInflectionPatternMap);
    // -----------------------------------------------------------------------------------------------------------------------

    if (addAffixPatternMappings) {
        // SUFFIX PATTERNS
        std::map<::std::u16string_view, int64_t> suffixToIdentifierRunsUncompressedMap;
        int32_t suffixesOffsetBitsUsed = 0;
        int32_t suffixCountBitsUsed = 0;

        // compress data
        for (const auto & [suffix, identifiers] : dictionary.suffixPatterns) {
            currentInflections.clear();
            for (const auto& identifier : identifiers) {
                currentInflections.emplace_back(identifierToInflectionPatternTrie->getKeyId(identifier.id));
            }
            int32_t suffixesOffset = suffixesToInflections.getOffset(currentInflections);
            int32_t suffixCount = int32_t(identifiers.size());
            suffixToIdentifierRunsUncompressedMap[suffix] = int64_t(suffixesOffset) << 32 | int64_t(suffixCount);
            suffixesOffsetBitsUsed |= suffixesOffset;
            suffixCountBitsUsed |= suffixCount;
        }
        numBitsForSuffixToIdentifierRunsIdx = calculateBitWidth(suffixesOffsetBitsUsed);
        numBitsForSuffixToIdentifierRunsLen = calculateBitWidth(suffixCountBitsUsed);

        if (numBitsForSuffixToIdentifierRunsIdx + numBitsForSuffixToIdentifierRunsLen > 32) {
            throw inflection::exception::IllegalStateException(u"Too many bits used for mapping suffixes to inflection patterns.");
        }

        // Compress the values used in the trie to use the fewest number of bits.
        std::map<::std::u16string_view, int32_t> suffixToIdentifierRunsCompressedMap;
        for (const auto & [suffix, data] : suffixToIdentifierRunsUncompressedMap) {
            suffixToIdentifierRunsCompressedMap[suffix] = int32_t(((uint64_t(data) >> 32) << numBitsForSuffixToIdentifierRunsLen) | (uint64_t(data) & uint64_t(0xFFFFFFFFLL)));
        }

        suffixToIdentifierRunsTrie = new inflection::dictionary::metadata::MarisaTrie<int32_t>(suffixToIdentifierRunsCompressedMap);
    }
    else {
        suffixToIdentifierRunsTrie = new inflection::dictionary::metadata::MarisaTrie<int32_t>(std::map<::std::u16string_view, int32_t>());
    }
}

InflectionDictionary::~InflectionDictionary() {
    delete inflection_Suffixes;
    delete identifierToInflectionPatternTrie;
    delete suffixToIdentifierRunsTrie;
}

int32_t InflectionDictionary::getId(std::u16string_view identifierStr) const
{
    return identifierToInflectionPatternTrie->getKeyId(identifierStr);
}

void InflectionDictionary::write(::std::ofstream& writer, DictionaryLogger& logger) const {
    writer.write(reinterpret_cast<const char*>(&inflection::dictionary::Inflector_MMappedDictionary::VERSION), sizeof(inflection::dictionary::Inflector_MMappedDictionary::VERSION));
    writer.write(reinterpret_cast<const char*>(&inflection::dictionary::Inflector_MMappedDictionary::ENDIANNESS_MARKER), sizeof(inflection::dictionary::Inflector_MMappedDictionary::ENDIANNESS_MARKER));
    // Write reserved options. It's currently unused.
    writer.write(reinterpret_cast<const char*>(&inflection::dictionary::Inflector_MMappedDictionary::OPTIONS), sizeof(inflection::dictionary::Inflector_MMappedDictionary::OPTIONS));

    logger.logWithOffset(locale.getName() + " header");

    int32_t grammemePatternsSize = int32_t(grammemePatterns.size());
    writer.write(reinterpret_cast<const char*>(&grammemePatternsSize), sizeof(grammemePatternsSize));
    logger.logWithOffset(locale.getName() + " grammemePatternsSize");
    writer.write(reinterpret_cast<const char*>(grammemePatterns.data()), grammemePatternsSize * sizeof(grammemePatterns[0]));
    logger.logWithOffset(locale.getName() + " grammemePatterns");

    npc(inflection_Suffixes)->write(writer);
    logger.logWithOffset(locale.getName() + " inflection_Suffixes number=" + std::to_string(npc(inflection_Suffixes)->size()));

    inflection::dictionary::metadata::CompressedArray compressedInflectionsArray(inflectionsArray);
    compressedInflectionsArray.serialize(writer);
    logger.logWithOffset(locale.getName() + " inflections number=" + std::to_string(inflectionsArray.size()));

    writer.write(reinterpret_cast<const char*>(&numBitsForGrammemesIdx), sizeof(numBitsForGrammemesIdx));
    logger.logWithOffset(locale.getName() + " numBitsForGrammemesIdx value=" + std::to_string(numBitsForGrammemesIdx));
    writer.write(reinterpret_cast<const char*>(&numBitsForSuffixIdx), sizeof(numBitsForSuffixIdx));
    logger.logWithOffset(locale.getName() + " numBitsForSuffixIdx value=" + std::to_string(numBitsForSuffixIdx));
    writer.write(reinterpret_cast<const char*>(&numBitsForSuffixToIdentifierRunsIdx), sizeof(numBitsForSuffixToIdentifierRunsIdx));
    logger.logWithOffset(locale.getName() + " numBitsForSuffixToIdentifierRunsIdx value=" + std::to_string(numBitsForSuffixToIdentifierRunsIdx));
    writer.write(reinterpret_cast<const char*>(&numBitsForSuffixToIdentifierRunsLen), sizeof(numBitsForSuffixToIdentifierRunsLen));
    logger.logWithOffset(locale.getName() + " numBitsForSuffixToIdentifierRunsLen value=" + std::to_string(numBitsForSuffixToIdentifierRunsLen));

    int32_t frequencyArraySize = int32_t(frequencyArray.size());
    writer.write(reinterpret_cast<const char*>(&frequencyArraySize), sizeof(frequencyArraySize));
    logger.logWithOffset(locale.getName() + " frequencyArraySize");
    writer.write(reinterpret_cast<const char*>(frequencyArray.data()), frequencyArraySize * sizeof(frequencyArray[0]));
    logger.logWithOffset(locale.getName() + " frequencyArray");

    npc(identifierToInflectionPatternTrie)->write(writer);
    logger.logWithOffset(locale.getName() + " identifierToInflectionPatternTrie");

    npc(suffixToIdentifierRunsTrie)->write(writer);
    logger.logWithOffset(locale.getName() + " suffixToIdentifierRunsTrie");

    inflection::dictionary::metadata::CompressedArray compressedSuffixesToInflectionsArray(suffixesToInflections.getSingletons());
    compressedSuffixesToInflectionsArray.serialize(writer);
    logger.logWithOffset(locale.getName() + " suffixesToInflections");

    writer.write(reinterpret_cast<const char*>(&numBitsForFrequencyIdx), sizeof(numBitsForFrequencyIdx));
    logger.logWithOffset(locale.getName() + " numBitsForFrequencyIdx value=" + std::to_string(numBitsForFrequencyIdx));
    writer.write(reinterpret_cast<const char*>(&numBitsForPOSIdx), sizeof(numBitsForPOSIdx));
    logger.logWithOffset(locale.getName() + " numBitsForPOSIdx value=" + std::to_string(numBitsForPOSIdx));
    writer.write(reinterpret_cast<const char*>(&numBitsForLemmaSuffixesLen), sizeof(numBitsForLemmaSuffixesLen));
    logger.logWithOffset(locale.getName() + " numBitsForLemmaSuffixesLen value=" + std::to_string(numBitsForLemmaSuffixesLen));
    writer.write(reinterpret_cast<const char*>(&numBitsForNumOfInflections), sizeof(numBitsForNumOfInflections));
    logger.logWithOffset(locale.getName() + " numBitsForNumOfInflections value=" + std::to_string(numBitsForNumOfInflections));
    // Reserved for future capability, and it's currently unused.
    int8_t numBitsForPrefixIdx = 0;
    writer.write(reinterpret_cast<const char*>(&numBitsForPrefixIdx), sizeof(numBitsForPrefixIdx));
    logger.logWithOffset(locale.getName() + " numBitsForPrefixIdx value=" + std::to_string(numBitsForPrefixIdx));
}

Inflector_InflectionDictionary* InflectionDictionary::getInflectionPatterns(const ::inflection::util::ULocale& locale, xmlDocPtr xmlDoc,
                                                                            const std::map<::std::u16string_view, int32_t>& grammemeStringIndices)
{
    auto dictionary = new Inflector_InflectionDictionary(locale);
    xmlNodePtr patterns = xmlDocGetRootElement(xmlDoc);
    if (patterns == nullptr) {
        throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <patterns>, got <null>");
    }
    if (xmlStrEqual(patterns->name, (const xmlChar *) "patterns") == 0) {
        throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <patterns>, got <" + ::inflection::util::StringUtils::to_u16string((const char *)patterns->name) + u">");
    }

    // value caches used for conversion and temporary states
    ::std::u16string avalue16;
    ::std::string avalue8;

    xmlNodePtr child = patterns->children;
    while (child != nullptr) {
        xmlNodePtr cur = child;
        child = child->next;
        if (cur->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(cur->name, (const xmlChar *) "pattern") == 0) {
            throw ::inflection::exception::XMLParseException(u"Expecting element <pattern>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(patterns->name))) + u">");
        }
        xmlChar* patternName = xmlGetProp(cur, (const xmlChar*) "name");
        if (patternName == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Attribute \"name\" not specified in <pattern> element");
        }
        std::u16string identifier16;
        ::inflection::util::StringUtils::convert(&identifier16, avalue8 = reinterpret_cast<const char*>(patternName));
        xmlFree(patternName);
        xmlChar* patternWords = xmlGetProp(cur, (const xmlChar*) "words");
        int32_t frequency = 0;
        if (patternWords != nullptr) {
            frequency = int32_t(std::strtol(reinterpret_cast<const char*>(patternWords), nullptr, 10));
            xmlFree(patternWords);
        }
        xmlNodePtr childChild = cur->children;
        auto insertionResult(npc(dictionary)->inflectionIdToPatternIdx.emplace(identifier16, npc(dictionary)->inflectionIdToPatternIdx.size()));
        if (!insertionResult.second) {
            throw ::inflection::exception::XMLParseException(u"Pattern name " + identifier16 + u" is not unique");
        }
        npc(dictionary)->inflectionPatterns.emplace_back(Inflector_InflectionPattern(identifier16, frequency, {}, {}));
        auto& inflectPattern = npc(dictionary)->inflectionPatterns.back();

        while (childChild != nullptr) {
            xmlNodePtr curChild = childChild;
            childChild = childChild->next;
            if (curChild->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(curChild->name, (const xmlChar *) "pos") != 0) {
                xmlChar* pos = xmlNodeGetContent(curChild);
                ::inflection::util::StringUtils::convert(&avalue16, avalue8 = reinterpret_cast<const char*>(pos));
                inflectPattern.partsOfSpeech |= inflection::dictionary::metadata::StringContainer::convertIdentifierToBit(getValue(grammemeStringIndices, avalue16));
                xmlFree(pos);
            }
            else if (xmlStrEqual(curChild->name, (const xmlChar *) "suffix") != 0) {
                xmlChar* newSuffix = xmlNodeGetContent(curChild);
                avalue16.clear();
                if (newSuffix != nullptr) {
                    ::inflection::util::StringUtils::convert(&avalue16, avalue8 = reinterpret_cast<const char*>(newSuffix));
                    xmlFree(newSuffix);
                }
                inflectPattern.lemmaSuffixes.emplace_back(avalue16);
            }
            else if (xmlStrEqual(curChild->name, (const xmlChar *) "inflections") != 0) {
                xmlNodePtr childChildChild = curChild->children;
                while (childChildChild != nullptr) {
                    xmlNodePtr curChildChild = childChildChild;
                    childChildChild = childChildChild->next;
                    if (curChildChild->type != XML_ELEMENT_NODE) {
                        continue;
                    }
                    if (xmlStrEqual(curChildChild->name, (const xmlChar *) "inflection") != 0) {
                        int64_t grammemes = 0;
                        for (xmlAttrPtr attr = curChildChild->properties; nullptr != attr; attr = attr->next) {
                            xmlChar* value = xmlNodeListGetString(xmlDoc, attr->children, 1);
                            ::inflection::util::StringUtils::convert(&avalue16, avalue8 = reinterpret_cast<const char*>(value));
                            xmlFree(value);
                            grammemes |= inflection::dictionary::metadata::StringContainer::convertIdentifierToBit(getValue(grammemeStringIndices, avalue16));
                        }
                        xmlChar *inflectionText = xmlNodeGetContent(curChildChild);
                        ::inflection::util::StringUtils::convert(&avalue16, reinterpret_cast<const char *>(inflectionText));
                        xmlFree(inflectionText);
                        inflectPattern.inflectionForGrammeme.emplace_back(avalue16, grammemes);
                        if (npc(dictionary)->forLemmatization) {
                            inflectPattern.lemmaSuffixes.emplace_back(avalue16);
                        }
                    }
                }
            }
        }

        for (const auto& stemSuf : inflectPattern.lemmaSuffixes) {
            auto sufIt = npc(dictionary)->suffixPatterns.find(stemSuf);
            if (sufIt == npc(dictionary)->suffixPatterns.end()) {
                sufIt = npc(dictionary)->suffixPatterns.emplace(stemSuf, ::std::set<Inflector_Suffix>()).first;
            }
            sufIt->second.insert(
                    Inflector_Suffix(inflectPattern.identifier, inflectPattern.frequency)
            );
        }
    }

    return dictionary;
}

Inflector_InflectionDictionary* InflectionDictionary::readXML(const ::inflection::util::ULocale& locale, const std::string& source,
                                                              const std::map<::std::u16string_view, int32_t>& grammemeStringIndices)
{
    Inflector_InflectionDictionary* result;
    LIBXML_TEST_VERSION

    xmlDocPtr xmlDoc = nullptr;
    {
        ::inflection::util::Finally finally([&xmlDoc]() noexcept {
            if (xmlDoc != nullptr) {
                xmlFreeDoc(xmlDoc);
            }
            xmlCleanupParser();
        });
        ::std::u16string resourceName = inflection::util::StringUtils::to_u16string(source);
        ::inflection::util::MemoryMappedFile mMapFile(resourceName);
        xmlDoc = xmlParseMemory(mMapFile.getData(), int32_t(mMapFile.getSize()));
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceName);
        }
        result = getInflectionPatterns(locale, xmlDoc, grammemeStringIndices);
    }

    return result;
}
