/*
 * Copyright 2024-2026 Apple Inc. All rights reserved.
 */
#include "InflectionDictionary.hpp"
#include "LexicalDictionaryBuilder.hpp"
#include "StringPool.hpp"
#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/util/Finally.hpp>
#include <inflection/npc.hpp>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Inflection {
    std::u16string_view suffix;
    int64_t grammemes;
};

struct Pattern {
    std::u16string_view identifier;
    int32_t frequency;
    int64_t partsOfSpeech;
    std::vector<std::u16string_view> lemmaSuffixes;
    std::vector<Inflection> inflectionForGrammeme;
};

struct ParsedInflectionData {
    std::vector<Pattern> patterns;
    StringPool suffixStringPool;
    std::set<std::u16string_view> inflectionSuffixesSet;
    std::map<int64_t, int32_t> grammemesSingletonsMap;
    std::set<int32_t> frequenciesSet;
    int32_t inflectionsTotalSize = 0;
    int32_t usedBitsForLemmaSuffixesLen = 0;
    int32_t usedBitsForNumOfInflections = 0;
};

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

InflectionDictionary::InflectionDictionary(const ::inflection::util::ULocale &locale)
    : locale(locale)
{
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

/*
 * libxml2 is a little too pedantic about the difference between UTF-8 strings and the unknown encoding of char* strings.
 * Also now that C11 & C++11 officially supports char8_t to mean UTF-8, xmlChar is now incompatible with the these standards for UTF-8 handling.
 * So we do a simple cast for these simple ASCII strings to make it easier to read.
 */
#define xutf8 reinterpret_cast<const xmlChar *>

static Pattern parsePatternElement(xmlDocPtr xmlDoc, xmlNodePtr cur,
                                   const std::map<::std::u16string_view, int32_t>& grammemeStringIndices,
                                   const StringPool& dictionaryStringPool,
                                   StringPool& suffixStringPool,
                                   std::set<::std::u16string_view>& inflectionSuffixesSet)
{
    ::std::u16string avalue16;

    xmlChar* patternName = xmlGetProp(cur, xutf8("name"));
    if (patternName == nullptr) {
        throw ::inflection::exception::XMLParseException(u"Attribute \"name\" not specified in <pattern> element");
    }
    std::u16string identifier16;
    ::inflection::util::StringViewUtils::convert(&identifier16, reinterpret_cast<const char*>(patternName));
    xmlFree(patternName);
    auto identifierOpt = dictionaryStringPool.find(identifier16);
    if (!identifierOpt) {
        throw ::inflection::exception::XMLParseException(u"Inflection pattern identifier not found in dictionary: " + identifier16);
    }
    auto identifierView = *identifierOpt;
    xmlChar* patternWords = xmlGetProp(cur, xutf8("words"));
    int32_t frequency = 0;
    if (patternWords != nullptr) {
        frequency = static_cast<int32_t>(std::strtol(reinterpret_cast<const char *>(patternWords), nullptr, 10));
        xmlFree(patternWords);
    }
    xmlNodePtr childChild = cur->children;
    Pattern inflectPattern{identifierView, frequency, 0, {}, {}};

    while (childChild != nullptr) {
        xmlNodePtr curChild = childChild;
        childChild = childChild->next;
        if (curChild->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(curChild->name, xutf8("pos")) != 0) {
            xmlChar* pos = xmlNodeGetContent(curChild);
            ::inflection::util::StringViewUtils::convert(&avalue16, reinterpret_cast<const char*>(pos));
            inflectPattern.partsOfSpeech |= inflection::dictionary::metadata::StringContainer::convertIdentifierToBit(getValue(grammemeStringIndices, avalue16));
            xmlFree(pos);
        }
        else if (xmlStrEqual(curChild->name, xutf8("suffix")) != 0) {
            xmlChar* newSuffix = xmlNodeGetContent(curChild);
            avalue16.clear();
            if (newSuffix != nullptr) {
                ::inflection::util::StringViewUtils::convert(&avalue16, reinterpret_cast<const char*>(newSuffix));
                xmlFree(newSuffix);
            }
            auto suffix = *inflectionSuffixesSet.insert(suffixStringPool.intern(avalue16)).first;
            inflectPattern.lemmaSuffixes.emplace_back(suffix);
        }
        else if (xmlStrEqual(curChild->name, xutf8("inflections")) != 0) {
            xmlNodePtr childChildChild = curChild->children;
            while (childChildChild != nullptr) {
                xmlNodePtr curChildChild = childChildChild;
                childChildChild = childChildChild->next;
                if (curChildChild->type != XML_ELEMENT_NODE) {
                    continue;
                }
                if (xmlStrEqual(curChildChild->name, xutf8("inflection")) != 0) {
                    int64_t grammemes = 0;
                    for (xmlAttrPtr attr = curChildChild->properties; nullptr != attr; attr = attr->next) {
                        xmlChar* value = xmlNodeListGetString(xmlDoc, attr->children, 1);
                        ::inflection::util::StringViewUtils::convert(&avalue16, reinterpret_cast<const char*>(value));
                        xmlFree(value);
                        grammemes |= inflection::dictionary::metadata::StringContainer::convertIdentifierToBit(getValue(grammemeStringIndices, avalue16));
                    }
                    xmlChar *inflectionText = xmlNodeGetContent(curChildChild);
                    ::inflection::util::StringViewUtils::convert(&avalue16, reinterpret_cast<const char *>(inflectionText));
                    xmlFree(inflectionText);
                    auto suffix = *inflectionSuffixesSet.insert(suffixStringPool.intern(avalue16)).first;
                    inflectPattern.inflectionForGrammeme.emplace_back(Inflection{suffix, grammemes});
                }
            }
        }
    }
    return inflectPattern;
}

static ParsedInflectionData parseXMLDocument(const std::string& source,
                                             const std::map<::std::u16string_view, int32_t>& grammemeStringIndices,
                                             const StringPool& dictionaryStringPool,
                                             InflectionDictionary* result)
{
    ParsedInflectionData parsed;

    LIBXML_TEST_VERSION

    xmlDocPtr xmlDoc = nullptr;
    inflection::util::Finally finally([&xmlDoc]() noexcept {
        if (xmlDoc != nullptr) {
            xmlFreeDoc(xmlDoc);
        }
        xmlCleanupParser();
    });
    std::u16string resourceName = inflection::util::StringViewUtils::to_u16string(source);
    inflection::util::MemoryMappedFile mMapFile(resourceName);
    auto xmlDocSize = mMapFile.getSize();
    xmlDoc = xmlParseMemory(mMapFile.getData(), static_cast<int32_t>(xmlDocSize));
    if (xmlDoc == nullptr) {
        throw ::inflection::exception::IOException(resourceName);
    }

    xmlNodePtr rootNode = xmlDocGetRootElement(xmlDoc);
    if (rootNode == nullptr) {
        throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <patterns>, got <null>");
    }
    if (xmlStrEqual(rootNode->name, xutf8("patterns")) == 0) {
        throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <patterns>, got <" + ::inflection::util::StringViewUtils::to_u16string(reinterpret_cast<const char *>(rootNode->name)) + u">");
    }

    xmlNodePtr child = rootNode->children;
    while (child != nullptr) {
        xmlNodePtr cur = child;
        child = child->next;
        if (cur->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(cur->name, xutf8("pattern")) == 0) {
            throw ::inflection::exception::XMLParseException(u"Expecting element <pattern>, got <" + ::inflection::util::StringViewUtils::to_u16string(reinterpret_cast<const char*>(rootNode->name)) + u">");
        }
        auto& inflectPattern = parsed.patterns.emplace_back(
            parsePatternElement(xmlDoc, cur, grammemeStringIndices, dictionaryStringPool,
                                parsed.suffixStringPool, parsed.inflectionSuffixesSet));

        // Collect statistics during parsing to avoid a separate pass
        parsed.frequenciesSet.insert(inflectPattern.frequency);
        result->getGrammemeSingletonIndex(parsed.grammemesSingletonsMap, inflectPattern.partsOfSpeech);
        parsed.inflectionsTotalSize += inflectPattern.inflectionForGrammeme.size();
        parsed.usedBitsForLemmaSuffixesLen |= inflectPattern.lemmaSuffixes.size();
        parsed.usedBitsForNumOfInflections |= inflectPattern.inflectionForGrammeme.size();
    }

    return parsed;
}

InflectionDictionary* InflectionDictionary::fromXML(const ::inflection::util::ULocale& locale,
                                                     const std::string& source,
                                                     const std::map<::std::u16string_view, int32_t>& grammemeStringIndices,
                                                     const StringPool& dictionaryStringPool)
{
    auto* result = new InflectionDictionary(locale);
    auto parsed = parseXMLDocument(source, grammemeStringIndices, dictionaryStringPool, result);
    result->buildCompressedData(parsed);
    return result;
}

void InflectionDictionary::buildCompressedData(ParsedInflectionData& parsed)
{
    std::map<::std::u16string_view, int32_t> identifierToInflectionPatternMap;
    std::map<int32_t, int32_t> frequenciesMap;

    numBitsForFrequencyIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(parsed.frequenciesSet.size());
    numBitsForPOSIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(parsed.grammemesSingletonsMap.size());
    numBitsForLemmaSuffixesLen = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(parsed.usedBitsForLemmaSuffixesLen);
    numBitsForNumOfInflections = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(parsed.usedBitsForNumOfInflections);

    frequencyArray.reserve(parsed.frequenciesSet.size());
    int32_t frequenciesCount = 0;
    for (auto frequency : parsed.frequenciesSet) {
        frequenciesMap.emplace(frequency, frequenciesCount);
        frequencyArray.emplace_back(frequency);
        frequenciesCount++;
    }

    inflectionsArray.reserve(parsed.inflectionsTotalSize + parsed.inflectionSuffixesSet.size());

    // Pass 1: Collect remaining grammeme singletons from inflections
    for (const auto& pattern : parsed.patterns) {
        for (const auto& inflection : pattern.inflectionForGrammeme) {
            getGrammemeSingletonIndex(parsed.grammemesSingletonsMap, inflection.grammemes);
        }
    }
    numBitsForGrammemesIdx = LexicalDictionaryBuilder::getNumBitsFromValues(parsed.grammemesSingletonsMap);

    // Pass 2: Build inflectionsArray
    std::map<::std::u16string_view, int32_t> inflection_SuffixesMap;
    inflection_Suffixes = new inflection::dictionary::metadata::StringContainer(parsed.inflectionSuffixesSet, &inflection_SuffixesMap);

    for (const auto& pattern : parsed.patterns) {
        if (!identifierToInflectionPatternMap.emplace(pattern.identifier, inflectionsArray.size()).second) {
            throw ::inflection::exception::XMLParseException(u"Pattern name " + std::u16string(pattern.identifier) + u" is not unique");
        }

        int32_t posIdx = getGrammemeSingletonIndex(parsed.grammemesSingletonsMap, pattern.partsOfSpeech);
        int64_t patternPrefix = frequenciesMap.find(pattern.frequency)->second;
        patternPrefix = (patternPrefix << numBitsForPOSIdx) | posIdx;
        patternPrefix = (patternPrefix << numBitsForLemmaSuffixesLen) | pattern.lemmaSuffixes.size();
        patternPrefix = (patternPrefix << numBitsForNumOfInflections) | pattern.inflectionForGrammeme.size();
        inflectionsArray.emplace_back(patternPrefix);

        for (const auto& lemmaSuffix : pattern.lemmaSuffixes) {
            inflectionsArray.emplace_back(inflection_SuffixesMap[lemmaSuffix]);
        }
        for (const auto& inflection : pattern.inflectionForGrammeme) {
            int32_t inflectionGrammemesIdx = getGrammemeSingletonIndex(parsed.grammemesSingletonsMap, inflection.grammemes);
            int64_t bits = (inflection_SuffixesMap[inflection.suffix] << numBitsForGrammemesIdx) | inflectionGrammemesIdx;
            inflectionsArray.emplace_back(bits);
        }
    }

    numBitsForSuffixIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::calculateBitWidth(inflection_SuffixesMap.size());

    if (numBitsForGrammemesIdx + numBitsForSuffixIdx > 64) {
        throw inflection::exception::IllegalStateException(u"Too many bits used for inflections.");
    }

    identifierToInflectionPatternTrie = new inflection::dictionary::metadata::MarisaTrie<int32_t>(identifierToInflectionPatternMap);
}

InflectionDictionary::~InflectionDictionary() {
    delete inflection_Suffixes;
    delete identifierToInflectionPatternTrie;
}

int32_t InflectionDictionary::getId(std::u16string_view identifierStr) const
{
    return identifierToInflectionPatternTrie->getKeyId(identifierStr);
}

template<typename T>
static void writeVal(::std::ofstream& writer, const T& value) {
    writer.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void InflectionDictionary::write(::std::ofstream& writer, DictionaryLogger& logger) const {
    writeVal(writer, inflection::dictionary::Inflector_MMappedDictionary::VERSION);
    writeVal(writer, inflection::dictionary::Inflector_MMappedDictionary::ENDIANNESS_MARKER);
    // Write reserved options. It's currently unused.
    writeVal(writer, inflection::dictionary::Inflector_MMappedDictionary::OPTIONS);

    logger.logWithOffset(locale.getName() + " header");

    int32_t grammemePatternsSize = int32_t(grammemePatterns.size());
    writeVal(writer, grammemePatternsSize);
    logger.logWithOffset(locale.getName() + " grammemePatternsSize");
    writer.write(reinterpret_cast<const char*>(grammemePatterns.data()), grammemePatternsSize * sizeof(grammemePatterns[0]));
    logger.logWithOffset(locale.getName() + " grammemePatterns");

    npc(inflection_Suffixes)->write(writer);
    logger.logWithOffset(locale.getName() + " inflection_Suffixes number=" + std::to_string(npc(inflection_Suffixes)->size()));

    inflection::dictionary::metadata::CompressedArray compressedInflectionsArray(inflectionsArray);
    compressedInflectionsArray.serialize(writer);
    logger.logWithOffset(locale.getName() + " inflections number=" + std::to_string(inflectionsArray.size()));

    writeVal(writer, numBitsForNumOfInflections);
    logger.logWithOffset(locale.getName() + " numBitsForNumOfInflections value=" + std::to_string(numBitsForNumOfInflections));
    writeVal(writer, numBitsForLemmaSuffixesLen);
    logger.logWithOffset(locale.getName() + " numBitsForLemmaSuffixesLen value=" + std::to_string(numBitsForLemmaSuffixesLen));
    writeVal(writer, numBitsForPOSIdx);
    logger.logWithOffset(locale.getName() + " numBitsForPOSIdx value=" + std::to_string(numBitsForPOSIdx));
    writeVal(writer, numBitsForFrequencyIdx);
    logger.logWithOffset(locale.getName() + " numBitsForFrequencyIdx value=" + std::to_string(numBitsForFrequencyIdx));

    writeVal(writer, numBitsForGrammemesIdx);
    logger.logWithOffset(locale.getName() + " numBitsForGrammemesIdx value=" + std::to_string(numBitsForGrammemesIdx));
    writeVal(writer, numBitsForSuffixIdx);
    logger.logWithOffset(locale.getName() + " numBitsForSuffixIdx value=" + std::to_string(numBitsForSuffixIdx));
    // Reserved for future capability, and it's currently unused.
    int8_t numBitsForPrefixIdx = 0;
    writeVal(writer, numBitsForPrefixIdx);
    logger.logWithOffset(locale.getName() + " numBitsForPrefixIdx value=" + std::to_string(numBitsForPrefixIdx));
    // This is reserved padding to be used in the future
    int8_t padding {  };
    writeVal(writer, padding);
    logger.logWithOffset(locale.getName() + " padding value=" + std::to_string(padding));

    // TODO This is future planned functionality to handle prefixes.
    std::map<::std::u16string_view, int32_t> mappingResult;
    inflection::dictionary::metadata::StringContainer inflectionPrefixes(std::set<::std::u16string_view>(), &mappingResult);
    inflectionPrefixes.write(writer);
    logger.logWithOffset(locale.getName() + " inflectionPrefixes value=" + std::to_string(inflectionPrefixes.size()));

    int32_t frequencyArraySize = static_cast<int32_t>(frequencyArray.size());
    int32_t paddedFrequencyArraySize = frequencyArraySize + ((frequencyArraySize + 1) % 2); // +1 for size
    writeVal(writer, paddedFrequencyArraySize);
    logger.logWithOffset(locale.getName() + " frequencyArraySize");
    writer.write(reinterpret_cast<const char*>(frequencyArray.data()), frequencyArraySize * sizeof(frequencyArray[0]));
    if (paddedFrequencyArraySize != frequencyArraySize) {
        int32_t padding = 0;
        writer.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    }
    logger.logWithOffset(locale.getName() + " frequencyArray");

    npc(identifierToInflectionPatternTrie)->write(writer);
    logger.logWithOffset(locale.getName() + " identifierToInflectionPatternTrie");
}
