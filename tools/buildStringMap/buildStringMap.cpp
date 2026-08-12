/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/BidirectionalStringMap.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/dictionary/metadata/StringContainer.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/uchar.h>
#include <iostream>
#include <fstream>

static bool VERBOSE = false;

static ::std::u16string_view trimWhitespace(::std::u16string_view str) {
    int32_t len = int32_t(str.length());
    int32_t start = 0;
    while (start < len && u_isWhitespace(str[start])) {
        start++;
    }
    int32_t end = len;
    while (end > start && u_isWhitespace(str[end - 1])) {
        end--;
    }
    return str.substr(start, end - start);
}

::std::u16string_view getNextItem(::inflection::util::DelimitedStringIterator& item, int64_t lineNum)
{
    if (!item.hasNext()) {
        std::cerr << "Incomplete mapping at line " << lineNum << std::endl;
        exit(-1);
    }
    auto result(inflection::util::StringViewUtils::trim(*item));
    if (result.empty()) {
        std::cerr << "Empty string at line " << lineNum << std::endl;
        exit(-1);
    }
    ++item;
    return result;
}

static int32_t validateMaps(const ::std::map<::std::u16string_view, ::std::u16string_view>& fromMap, const ::std::map<::std::u16string_view, ::std::u16string_view>& toMap) {
    int32_t errorCount = 0;
    for (const auto& [fromStr, toStr] : fromMap) {
        if (toMap.find(toStr) == toMap.end()) {
            std::cerr << "String \"" << inflection::util::StringViewUtils::to_string(toStr) << "\" does not have a return mapping" << std::endl;
            errorCount++;
        }
    }
    for (const auto& [toStr, fromStr] : toMap) {
        if (fromMap.find(fromStr) == fromMap.end()) {
            std::cerr << "String \"" << inflection::util::StringViewUtils::to_string(fromStr) << "\" does not have a return mapping" << std::endl;
            errorCount++;
        }
    }
    return errorCount;
}

static int32_t convertStringValuesToIdentifiers(const ::std::map<::std::u16string_view, ::std::u16string_view>& map, const ::std::map<::std::u16string_view, int32_t>& mapValuesToId, ::std::map<::std::u16string_view, int32_t>& mapKeysToId) {
    int32_t errorCount = 0;
    for (const auto& [fromStr, toStr] : map) {
        auto entry(mapValuesToId.find(toStr));
        if (entry == mapValuesToId.end()) {
            std::cerr << "String \"" << inflection::util::StringViewUtils::to_string(toStr) << "\" does not have an identifier" << std::endl;
            errorCount++;
        }
        mapKeysToId.emplace(fromStr, entry->second);
    }
    return errorCount;
}

static ::icu4cxx::UnicodeSet convertToUnicodeSet(const ::std::set<::std::u16string_view>& keySet)
{
    ::icu4cxx::UnicodeSet relevantSet;
    for (const auto& word : keySet) {
        relevantSet.addAllCodePoints(word);
    }
    return relevantSet;
}

static ::std::u16string convertToPattern(const ::icu4cxx::UnicodeSet& unicodeSet)
{
    char16_t pattern[1024];
    int32_t size = unicodeSet.toPattern((UChar*)pattern, sizeof(pattern)/sizeof(pattern[0]), false);
    return ::std::u16string(pattern, size);
}

static bool caseClosure(::icu4cxx::UnicodeSet& relevantSet)
{
    auto sizeBeforeCaseFolding = relevantSet.size();
    relevantSet.closeOver(USET_CASE_INSENSITIVE);
    auto sizeAfterCaseFolding = relevantSet.size();
    return sizeBeforeCaseFolding != sizeAfterCaseFolding;
}

static constexpr int16_t ENDIANNESS_MARKER = 1;
static constexpr int16_t ALIGNMENT_SIZE = 8;

void writeHeader(std::ofstream& out, int8_t numberOfTries, const ::std::set<::std::u16string_view>& fromSet, const ::std::set<::std::u16string_view>& toSet) {
    ::icu4cxx::UnicodeSet fromUnicodeSet(convertToUnicodeSet(fromSet));
    ::icu4cxx::UnicodeSet toUnicodeSet(convertToUnicodeSet(toSet));
    auto fromPattern(convertToPattern(fromUnicodeSet));
    auto toPattern(convertToPattern(toUnicodeSet));

    if (VERBOSE) {
        std::cout << "From set: " << inflection::util::StringViewUtils::to_string(fromPattern) << " To set: " << inflection::util::StringViewUtils::to_string(toPattern)<< std::endl;
    }

    out.write(inflection::grammar::BidirectionalStringMap::MAGIC_MARKER, sizeof(inflection::grammar::BidirectionalStringMap::MAGIC_MARKER));
    out.write(reinterpret_cast<const char*>(&inflection::grammar::BidirectionalStringMap::VERSION), sizeof(inflection::grammar::BidirectionalStringMap::VERSION));
    out.write(reinterpret_cast<const char*>(&ENDIANNESS_MARKER), sizeof(ENDIANNESS_MARKER));
    out.write(reinterpret_cast<const char*>(&numberOfTries), sizeof(numberOfTries));
    uint8_t options = 0;
    if (caseClosure(fromUnicodeSet)) {
        options |= (uint8_t)inflection::grammar::BidirectionalStringMap::Options::FROM_CASE_SENSITIVE;
    }
    if (caseClosure(toUnicodeSet)) {
        options |= (uint8_t)inflection::grammar::BidirectionalStringMap::Options::TO_CASE_SENSITIVE;
    }
    out.write(reinterpret_cast<const char*>(&options), sizeof(options));

    char reserved[inflection::grammar::BidirectionalStringMap::RESERVED_BYTES] = {  };
    out.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));

    std::string fromPatternUTF8(inflection::util::StringViewUtils::to_string(fromPattern));
    std::string toPatternUTF8(inflection::util::StringViewUtils::to_string(toPattern));

    fromPatternUTF8.resize(fromPatternUTF8.length() + ALIGNMENT_SIZE - (fromPatternUTF8.size() % ALIGNMENT_SIZE), 0);
    toPatternUTF8.resize(toPatternUTF8.length() + ALIGNMENT_SIZE - (toPatternUTF8.size() % ALIGNMENT_SIZE), 0);

    int32_t fromSetStrLen = int32_t(fromPatternUTF8.length());
    out.write(reinterpret_cast<const char*>(&fromSetStrLen), sizeof(fromSetStrLen));
    out.write(reinterpret_cast<const char*>(fromPatternUTF8.data()), fromSetStrLen);
    int32_t toSetStrLen = int32_t(toPatternUTF8.length());
    out.write(reinterpret_cast<const char*>(&toSetStrLen), sizeof(toSetStrLen));
    out.write(reinterpret_cast<const char*>(toPatternUTF8.data()), toSetStrLen);
}

void writeBidirectionalTries(const char* outFileName,
                             const ::std::set<::std::u16string_view>& fromSet,
                             const ::std::set<::std::u16string_view>& toSet,
                             const ::std::map<::std::u16string_view, ::std::u16string_view>& fromMap,
                             const ::std::map<::std::u16string_view, ::std::u16string_view>& toMap)
{
    ::std::map<::std::u16string_view, int32_t> fromSetMap;
    {
        inflection::dictionary::metadata::StringContainer fromContainer(fromSet, &fromSetMap);
    }
    ::std::map<::std::u16string_view, int32_t> toSetMap;
    {
        inflection::dictionary::metadata::StringContainer toContainer(toSet, &toSetMap);
    }

    ::std::map<::std::u16string_view, int32_t> fromIdMap;
    convertStringValuesToIdentifiers(fromMap, toSetMap, fromIdMap);
    ::inflection::dictionary::metadata::MarisaTrie fromTrie(fromIdMap);

    ::std::map<::std::u16string_view, int32_t> toIdMap;
    convertStringValuesToIdentifiers(toMap, fromSetMap, toIdMap);
    ::inflection::dictionary::metadata::MarisaTrie toTrie(toIdMap);

    std::ofstream out(outFileName, std::ios::binary);
    if (!out) {
        std::cerr << "Unable to open output file: " << outFileName << std::endl;
        exit(-1);
    }
    writeHeader(out, 2, fromSet, toSet);
    fromTrie.write(out);
    toTrie.write(out);
}

void writeSelfReferencingTrie(const char* outFileName,
                             const ::std::set<::std::u16string_view>& fromSet,
                             const ::std::set<::std::u16string_view>& toSet,
                             const ::std::map<::std::u16string_view, ::std::u16string_view>& fromMap,
                             const ::std::map<::std::u16string_view, ::std::u16string_view>& toMap)
{
    ::std::set<::std::u16string_view> combinedSet(fromSet);
    combinedSet.insert(toSet.begin(), toSet.end());

    ::std::map<::std::u16string_view, int32_t> combinedSetMap;
    {
        inflection::dictionary::metadata::StringContainer fromContainer(combinedSet, &combinedSetMap);
    }

    ::std::map<::std::u16string_view, int32_t> combinedIdMap;
    convertStringValuesToIdentifiers(fromMap, combinedSetMap, combinedIdMap);
    convertStringValuesToIdentifiers(toMap, combinedSetMap, combinedIdMap);
    ::inflection::dictionary::metadata::MarisaTrie combinedTrie(combinedIdMap);

    std::ofstream out(outFileName, std::ios::binary);
    if (!out) {
        std::cerr << "Unable to open output file: " << outFileName << std::endl;
        exit(-1);
    }
    writeHeader(out, 1, fromSet, toSet);
    combinedTrie.write(out);
}

static constexpr int32_t MINIMUM_SIZE_FOR_DUAL_TRIE = 256;
static constexpr char USAGE_STRING[] =
    "Usage: buildStringMap INFILE OUTFILE";

int main(int argc, const char * const argv[]) {
    if (argc < 3) {
        std::cout << USAGE_STRING << std::endl;
        return -1;
    }

    std::string_view inFileName(argv[1]);
    const char* outFileName(argv[2]);

    inflection::util::MemoryMappedFile in(inflection::util::StringViewUtils::to_u16string(inFileName));

    ::std::string_view u8contents(in.getData(), in.getSize());
    ::std::u16string fileContents;
    inflection::util::StringViewUtils::lowercase(&fileContents, inflection::util::StringViewUtils::to_u16string(u8contents), inflection::util::LocaleUtils::ROOT());

    int32_t lineNum = 0;
    int32_t errorCount = 0;
    ::std::set<::std::u16string_view> fromSet;
    ::std::set<::std::u16string_view> toSet;
    ::std::map<::std::u16string_view, ::std::u16string_view> fromMap;
    ::std::map<::std::u16string_view, ::std::u16string_view> toMap;

    for (::inflection::util::DelimitedStringIterator iterator(fileContents, u"\n"); iterator.hasNext(); ++iterator) {
        auto line(trimWhitespace(*iterator));
        lineNum++;
        if (line.empty()) {
            continue;
        }
        ::inflection::util::DelimitedStringIterator item(line, u",");
        ::std::u16string_view fromStr(getNextItem(item, lineNum));
        ::std::u16string_view operatorStr(getNextItem(item, lineNum));
        ::std::u16string_view toStr(getNextItem(item, lineNum));
        fromSet.insert(fromStr);
        toSet.insert(toStr);
        if (operatorStr == u"=") {
            if (!fromMap.emplace(fromStr, toStr).second) {
                std::cerr << "The string " <<  inflection::util::StringViewUtils::to_string(fromStr) << " in \"" << inflection::util::StringViewUtils::to_string(line) << "\" is a conflicting mapping at line " << lineNum << std::endl;
                errorCount++;
            }
            if (!toMap.emplace(toStr, fromStr).second) {
                std::cerr << "The string " <<  inflection::util::StringViewUtils::to_string(toStr) << " in \"" << inflection::util::StringViewUtils::to_string(line) << "\" is a conflicting mapping at line " << lineNum << std::endl;
                errorCount++;
            }
        }
        else if (operatorStr == u">") {
            if (!fromMap.emplace(fromStr, toStr).second) {
                std::cerr << "The string " <<  inflection::util::StringViewUtils::to_string(fromStr) << " in \"" << inflection::util::StringViewUtils::to_string(line) << "\" is a conflicting mapping at line " << lineNum << std::endl;
                errorCount++;
            }
        }
        else if (operatorStr == u"<") {
            if (!toMap.emplace(toStr, fromStr).second) {
                std::cerr << "The string " <<  inflection::util::StringViewUtils::to_string(toStr) << " in \"" << inflection::util::StringViewUtils::to_string(line) << "\" is a conflicting mapping at line " << lineNum << std::endl;
                errorCount++;
            }
        }
        else {
            std::cerr << "Unknown operator \"" << inflection::util::StringViewUtils::to_string(operatorStr) << "\" at line " << lineNum << std::endl;
            errorCount++;
        }
    }

    errorCount += validateMaps(fromMap, toMap);
    if (errorCount > 0) {
        std::cerr << errorCount << " Errors" << std::endl;
        return -1;
    }

    ::std::map<::std::u16string_view, int32_t> fromSetMap;
    {
        inflection::dictionary::metadata::StringContainer fromContainer(fromSet, &fromSetMap);
    }
    ::std::map<::std::u16string_view, int32_t> toSetMap;
    {
        inflection::dictionary::metadata::StringContainer toContainer(toSet, &toSetMap);
    }

    ::std::map<::std::u16string_view, int32_t> fromIdMap;
    convertStringValuesToIdentifiers(fromMap, toSetMap, fromIdMap);
    ::inflection::dictionary::metadata::MarisaTrie fromTrie(fromIdMap);

    ::std::map<::std::u16string_view, int32_t> toIdMap;
    convertStringValuesToIdentifiers(toMap, fromSetMap, toIdMap);
    ::inflection::dictionary::metadata::MarisaTrie toTrie(toIdMap);

    std::ofstream out(outFileName, std::ios::binary);
    if (!out) {
        std::cerr << "Unable to open output file: " << outFileName << std::endl;
        return -1;
    }

    if (fromSet.size() + toSet.size() < MINIMUM_SIZE_FOR_DUAL_TRIE) {
        writeSelfReferencingTrie(outFileName, fromSet, toSet, fromMap, toMap);
    }
    else {
        writeBidirectionalTries(outFileName, fromSet, toSet, fromMap, toMap);
    }

    return 0;
}
