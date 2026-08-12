/*
 * Copyright 2021-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiTokenExtractorIterator.hpp>

#include <inflection/exception/ExcessComplexityException.hpp>
#include <inflection/tokenizer/locale/fi/FiCompound.hpp>
#include <inflection/tokenizer/locale/fi/FiSegment.hpp>
#include <inflection/tokenizer/locale/fi/FiSuffixSplit.hpp>
#include <inflection/tokenizer/locale/fi/FiTokenizerConfig.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::locale {
namespace fi {

FiTokenExtractorIterator::FiTokenExtractorIterator(const inflection::tokenizer::trie::SerializedTrie& corpus, std::u16string_view str)
    : corpus(corpus)
    , tokenExtractorIterator(str.data(), int32_t(str.length()))
    , charSequence(str)
{
}

FiTokenExtractorIterator::~FiTokenExtractorIterator()
{
}

static constexpr int32_t MAXIMUM_DECOMPOUND_MORPHEMES = 14;

/*
* This is trying to lowercase without changing the length of each character.
*/
static void lowerCase(::std::u16string* lowerStr, std::u16string_view str, int32_t start, int32_t end)
{
    npc(lowerStr)->reserve(end - start);
    for (auto idx = start; idx < end; idx++) {
        ::inflection::util::StringUtils::appendCodePoint(lowerStr, u_tolower(str[idx]));
    }
}

int32_t FiTokenExtractorIterator::getNextBoundary()
{
    if (boundariesIdx < (int32_t)boundaries.size()) {
        return boundaries[boundariesIdx++];
    }
    boundaries.clear();
    boundariesIdx = 0;
    int32_t boundary;
    if ((boundary = tokenExtractorIterator.getNextBoundary()) != UBRK_DONE) {
        if (boundary - lastStart > FiTokenizerConfig::MINSTEMLENGTH) {
            sbWord.clear();
            lowerCase(&sbWord, charSequence, lastStart, boundary);
            auto length = int32_t(sbWord.length());
            FiSegment segmentWord(corpus, sbWord, 0, length, true);
            if (segmentWord.found()) {
                segmentWord.addBoundaries(&boundaries);
            } else if (length >= FiTokenizerConfig::MIN_FISUFFIXSPLIT) {
                FiSuffixSplit suffixSplit(&segmentWord);
                suffixSplit.addBoundaries(&boundaries, FiSuffixSplit::SuffixType::SUFFIX);
                if (!suffixSplit.found() && length >= FiTokenizerConfig::MIN_FICOMPOUNDSPLIT) {
                    FiCompound fiCompound(&suffixSplit);
                    boundaries.clear();
                    try {
                        fiCompound.addBoundaries(MAXIMUM_DECOMPOUND_MORPHEMES, &boundaries);
                    }
                    catch (const ::inflection::exception::ExcessComplexityException &) {
                        if (::inflection::util::LoggerConfig::isDebugEnabled()) {
                            ::inflection::util::Logger::debug(::std::u16string(u"The token \"") + sbWord
                                                           + ::std::u16string(u"\" is too complex to decompound"));
                        }
                    }
                }
            }
            if (!boundaries.empty()) {
                for (auto& breakpoint : boundaries) {
                    breakpoint += lastStart;
                }
                boundaries.emplace_back(boundary);
                lastStart = boundary;
                boundary = boundaries[boundariesIdx++];
            }
            else {
                lastStart = boundary;
            }
        }
        else {
            lastStart = boundary;
        }
    }
    return boundary;
}

void FiTokenExtractorIterator::reset(std::u16string_view str)
{
    charSequence = str;
    lastStart = 0;
    tokenExtractorIterator.reset(str);
    boundaries.clear();
    boundariesIdx = 0;
}

} // namespace tokenizer::locale::fi
} // namespace inflection
