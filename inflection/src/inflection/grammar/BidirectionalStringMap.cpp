/*
 * Copyright 2022-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/BidirectionalStringMap.hpp>

#include <inflection/exception/IOException.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar {

static constexpr int32_t MAGIC_MARKER_LEN = sizeof(BidirectionalStringMap::MAGIC_MARKER);
static constexpr int16_t ENDIANNESS_MARKER = 1;

BidirectionalStringMap::BidirectionalStringMap(const std::u16string& pathToFile)
    : super()
    , mappedFile(pathToFile)
    , numberOfTries(validateHeader(mappedFile, pathToFile))
    , options(getOptions(mappedFile))
    , sourceSet(getFromUnicodeSetPattern(mappedFile))
    , targetSet(getFromUnicodeSetPattern(mappedFile))
    , sourceTrie(&mappedFile)
    , dynamicTrie(numberOfTries == 2 ? new inflection::dictionary::metadata::MarisaTrie<int32_t>(&mappedFile) : nullptr)
    , targetTrie(dynamicTrie == nullptr ? sourceTrie : *npc(dynamicTrie))
{
    if ((options & (uint8_t)Options::FROM_CASE_SENSITIVE) != 0) {
        sourceSet.closeOver(USET_CASE_INSENSITIVE);
    }
    inflection::util::UnicodeSetUtils::freeze(&sourceSet);
}

BidirectionalStringMap::~BidirectionalStringMap()
{
    if (dynamicTrie != nullptr) {
        delete dynamicTrie;
    }
}

int8_t BidirectionalStringMap::validateHeader(::inflection::util::MemoryMappedFile& mappedFile, const std::u16string& pathToFile)
{
    const char* magicMarker;
    mappedFile.read(&magicMarker, MAGIC_MARKER_LEN);
    if (strncmp(magicMarker, MAGIC_MARKER, MAGIC_MARKER_LEN) != 0) {
        throw ::inflection::exception::IOException(u"Input file " + pathToFile + u" has an invalid header");
    }

    if (mappedFile.read<int64_t>() != VERSION) {
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

uint8_t BidirectionalStringMap::getOptions(::inflection::util::MemoryMappedFile &mappedFile)
{
    auto options = mappedFile.read<uint8_t>();

    const char* reserved;
    mappedFile.read(&reserved, RESERVED_BYTES);

    return options;
}

std::u16string BidirectionalStringMap::getFromUnicodeSetPattern(::inflection::util::MemoryMappedFile &mappedFile)
{
    auto patternSize = mappedFile.read<uint32_t>();

    const char* patternPtr;
    mappedFile.read(&patternPtr, patternSize);

    std::u16string pattern;
    std::u16string result;
    inflection::util::StringViewUtils::convert(&pattern, std::string_view(patternPtr, patternSize));
    inflection::util::StringViewUtils::replace(&result, pattern, std::u16string_view(u"\0", 1), u"");
    return result;
}

const ::icu4cxx::UnicodeSet& BidirectionalStringMap::getSourceSet() const
{
    return sourceSet;
}

const ::icu4cxx::UnicodeSet& BidirectionalStringMap::getTargetSet() const
{
    return targetSet;
}

uint8_t BidirectionalStringMap::getOptions() const
{
    return options;
}

std::optional<std::u16string> BidirectionalStringMap::findTarget(std::u16string_view sourceString) const
{
    auto result = sourceTrie.find(sourceString);
    if (result) {
        return targetTrie.getKey(*result);
    }
    return {};
}

std::optional<std::u16string> BidirectionalStringMap::findSource(std::u16string_view targetString) const
{
    auto result = targetTrie.find(targetString);
    if (result) {
        return sourceTrie.getKey(*result);
    }
    return {};
}

} // namespace inflection::grammar
