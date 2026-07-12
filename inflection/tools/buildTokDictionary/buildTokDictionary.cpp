/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/util/StringUtils.hpp>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <iostream>
#include <fstream>

static std::u16string_view trimWhitespace(std::u16string_view str) {
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

static const uint8_t digitInfo[] = {
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,
        0x80u, 0x81u, 0x82u, 0x83u, 0x84u, 0x85u, 0x86u, 0x87u,
        0x88u, 0x89u,     0,     0,     0,     0,     0,     0,
        0, 0x8au, 0x8bu, 0x8cu, 0x8du, 0x8eu, 0x8fu, 0x90u,
        0x91u, 0x92u, 0x93u, 0x94u, 0x95u, 0x96u, 0x97u, 0x98u,
        0x99u, 0x9au, 0x9bu, 0x9cu, 0x9du, 0x9eu, 0x9fu, 0xa0u,
        0xa1u, 0xa2u, 0xa3u,     0,     0,     0,     0,     0,
        0, 0x8au, 0x8bu, 0x8cu, 0x8du, 0x8eu, 0x8fu, 0x90u,
        0x91u, 0x92u, 0x93u, 0x94u, 0x95u, 0x96u, 0x97u, 0x98u,
        0x99u, 0x9au, 0x9bu, 0x9cu, 0x9du, 0x9eu, 0x9fu, 0xa0u,
        0xa1u, 0xa2u, 0xa3u,     0,     0,     0,     0,     0,
};

int64_t utoi(std::u16string_view str, uint8_t radix)
{
    if (radix > 36) {
        radix = 36;
    } else if (radix < 2) {
        radix = 2;
    }
    int64_t lradix = radix;

    bool neg = false;
    int32_t strIdx = 0;
    int32_t strLen = int32_t(str.length());
    if (str[0] == u'-') {
        strIdx++;
        neg = true;
    }
    int64_t result = 0;
    char16_t c;
    uint8_t b;
    while (strIdx < strLen && ((c = str[strIdx]) < 0x0080) && (b = digitInfo[c]) && ((b &= 0x7f) < radix)) {
        result *= lradix;
        result += b;
        strIdx++;
    }
    if (neg) {
        result = -result;
    }
    return result;
}

char16_t *strdup(std::u16string_view str) {
    auto len = int32_t(str.length() + 1);
    auto result = new char16_t[len];
    u_memcpy((UChar *)result, (const UChar *)str.data(), len);
    return result;
}
static constexpr int32_t INITIAL_STRING_SINGLETON_SIZE = 256*1024;
static constexpr int16_t ENDIANNESS_MARKER = 1;
static constexpr char USAGE_STRING[] =
    "Usage: buildTokDictionary INFILE OUTFILE";

int main(int argc, const char * const argv[]) {
    // If no command line arguments, print usage and exit
    if (argc < 3) {
        std::cout << USAGE_STRING << std::endl;
    }

    std::string inFileName(argv[1]);
    std::string outFileName(argv[2]);
    std::ifstream in(inFileName);
    if (!in) {
        std::cerr << "Unable to open input file: " << inFileName << std::endl;
        return -1;
    }

    std::ofstream out(outFileName, std::ios::binary);
    if (!out) {
        std::cerr << "Unable to open output file: " << outFileName << std::endl;
        in.close();
        return -1;
    }

    ::std::string line;
    ::std::u16string u16Line;
    ::std::u16string word;
    ::std::u16string_view valueStr;
    ::std::map<::std::u16string_view, int32_t> stringToIntegerMap;
    ::std::vector<const char16_t*> stringSingletons;
    int32_t errorCount = 0;

    stringSingletons.reserve(INITIAL_STRING_SINGLETON_SIZE);

    while (std::getline(in, line)) {
        ::inflection::util::StringUtils::convert(&u16Line, line);
        auto trimmedLine(trimWhitespace(u16Line));
        if (trimmedLine.empty()) {
            continue;
        }
        int32_t wordLen = 0;
        int32_t trimmedLen = int32_t(trimmedLine.length());
        while (wordLen < trimmedLen && !u_isWhitespace(trimmedLine[wordLen])) {
            wordLen++;
        }
        word = trimmedLine.substr(0, wordLen);

        valueStr = trimWhitespace(trimmedLine.substr(wordLen));
        if (valueStr.length() != 8) {
            errorCount++;
            std::cerr << "Line is does not have expected number of digits: " << line << std::endl;
        }
        int32_t value = int32_t(utoi(valueStr, 16));

        auto wordChars = strdup(word);
        stringSingletons.emplace_back(wordChars); // Keep them around for u16string_view
        auto previousEntry = stringToIntegerMap.emplace(wordChars, value);
        if (!previousEntry.second) {
            errorCount++;
            std::cerr << "Duplicate entry: " << inflection::util::StringUtils::to_string(word) << std::endl;
        }
    }

    if (errorCount == 0) {
        ::inflection::dictionary::metadata::MarisaTrie<int32_t> trie(stringToIntegerMap);
        out.write(inflection::tokenizer::trie::SerializedTrie::MAGIC_MARKER, sizeof(inflection::tokenizer::trie::SerializedTrie::MAGIC_MARKER));
        out.write(reinterpret_cast<const char*>(&inflection::tokenizer::trie::SerializedTrie::VERSION), sizeof(inflection::tokenizer::trie::SerializedTrie::VERSION));
        out.write(reinterpret_cast<const char*>(&ENDIANNESS_MARKER), sizeof(ENDIANNESS_MARKER));
        uint16_t options = 0;
        out.write(reinterpret_cast<const char*>(&options), sizeof(options));
        // Space reserved for future use. Also used to align data structures after this header.
        const char reserved[inflection::tokenizer::trie::SerializedTrie::RESERVED_BYTES] = {};
        out.write(reserved, sizeof(reserved));
        trie.write(out);
    }

    for (auto str : stringSingletons) {
        delete []str;
    }

    return errorCount;
}
