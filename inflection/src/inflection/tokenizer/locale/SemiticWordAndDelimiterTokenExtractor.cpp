/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor.hpp>

#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/locale/SemiticTokenExtractorIterator.hpp>
#include <inflection/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor_AffixRecord.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::locale {

static constexpr char16_t PREFIX_KEY[] = { u"tokenizer.prefixes" };
static constexpr char16_t SUFFIX_KEY[] = { u"tokenizer.suffixes" };
static constexpr char16_t NORMALIZE_KEY[] = { u"tokenizer.normalize" };
static constexpr char16_t REPLACE_KEY[] = { u"tokenizer.replace" };

SemiticWordAndDelimiterTokenExtractor::SemiticWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::icu4cxx::UnicodeSet& nativeSet, const ::std::map<::std::u16string_view, const char16_t*>& config)
    : super(locale, config)
    , nativeSet(nativeSet)
{
    auto prefixSize = getPrefixClasses(config);
    if (prefixSize > 0) {
        prefixTree.resize(prefixSize);
        addPrefixes(config);
    }
    addSuffixes(config);
    auto norm = config.find(NORMALIZE_KEY);
    if (norm != config.end()) {
        toNormalize = norm->second;
    }
    auto rep = config.find(REPLACE_KEY);
    if (rep != config.end()) {
        replacement = rep->second;
    }
}

inflection::tokenizer::iterator::TokenExtractorIterator* SemiticWordAndDelimiterTokenExtractor::createIterator(std::u16string_view str) const
{
    return new inflection::tokenizer::locale::SemiticTokenExtractorIterator(*this, str);
}

bool SemiticWordAndDelimiterTokenExtractor::isKnownWord(std::u16string_view compoundWord) const
{
    return wordsToNotSplit != nullptr && wordsToNotSplit->find(compoundWord) != wordsToNotSplit->end();
}

void
SemiticWordAndDelimiterTokenExtractor::decompoundWord(std::vector<int32_t>* boundaries, std::u16string_view charSequence, int32_t start, int32_t end) const
{
    std::u16string_view compoundWord(charSequence.substr(start, end - start));
    auto originalLength = int32_t(compoundWord.length());
    if (originalLength <= MINIMUM_WORD_LENGTH || !::inflection::util::UnicodeSetUtils::containsSome(nativeSet, compoundWord) || isKnownWord(compoundWord)) {
        return;
    }
    int32_t prefixIdx;
    auto newLength = originalLength;
    int32_t lastPrefixIdx = 0;
    auto possibleWord = compoundWord;
    int32_t prefixCount = 0;
    for (const auto& affixRecord : prefixTree) {
        if (newLength <= MINIMUM_WORD_LENGTH || isKnownWord(possibleWord)) {
            break;
        }
        auto lastGoodNode = &affixRecord;
        auto node = &affixRecord;
        prefixIdx = lastPrefixIdx;
        auto currentLength = newLength;
        while (currentLength > MINIMUM_WORD_LENGTH) {
            if ((node = npc(node)->getNextAffix(compoundWord[prefixIdx])) == nullptr) {
                break;
            }
            lastGoodNode = node;
            currentLength--;
            prefixIdx++;
        }
        if (npc(lastGoodNode)->isFinal) {
            newLength = currentLength;
            possibleWord = compoundWord.substr(prefixIdx, newLength);
            npc(boundaries)->emplace_back(prefixIdx + start);
            lastPrefixIdx = prefixIdx;
            prefixCount++;
        }
    }
    if (!isKnownWord(possibleWord)) {
        auto lastGoodNode = &suffixTree;
        auto node = &suffixTree;
        auto suffixIdx = originalLength;
        auto prefixSize = originalLength - newLength;
        while (newLength > MINIMUM_WORD_LENGTH) {
            if ((node = npc(node)->getNextAffix(compoundWord[--suffixIdx])) == nullptr) {
                break;
            }
            lastGoodNode = node;
            newLength--;
        }
        if (npc(lastGoodNode)->isFinal) {
            lastPrefixIdx = newLength + prefixSize;
            if (newLength != originalLength && prefixCount > 1) {
                // TODO this overtokenizes. This may not be a good idea, but it improves consistency.
                auto boundariesSize = npc(boundaries)->size();
                decompoundWord(boundaries, charSequence, prefixSize + start, lastPrefixIdx + start);
                if (npc(boundaries)->size() == boundariesSize) {
                    // Things didn't change. Confirm what we have so far.
                    npc(boundaries)->emplace_back(lastPrefixIdx + start);
                }
            }
            else {
                npc(boundaries)->emplace_back(newLength + prefixSize + start);
            }
        }
    }
    if (lastPrefixIdx != 0 && prefixCount > 1) {
        // TODO this overtokenizes. This may not be a good idea, but it improves consistency.
        return decompoundWord(boundaries, charSequence, lastPrefixIdx + start, end);
    }
    if (!npc(boundaries)->empty()) {
        npc(boundaries)->emplace_back(end);
    }
}

::std::u16string* SemiticWordAndDelimiterTokenExtractor::normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view fullCharSequence, int32_t start) const
{
    if (!toNormalize.empty() && src.ends_with(toNormalize)
        && (start + src.length()) < fullCharSequence.length() && nativeSet.contains(fullCharSequence[start + src.length()]))
    {
        npc(dest)->assign(src.substr(0, src.length() - toNormalize.length()));
        npc(dest)->append(replacement);
        return dest;
    }
    return super::normalize(dest, src, fullCharSequence, start);
}

int32_t SemiticWordAndDelimiterTokenExtractor::getPrefixClasses(const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    int32_t number = 0;
    auto prefixes = config.find(::std::u16string(PREFIX_KEY) + u"." + ::inflection::util::StringUtils::to_u16string(number));
    while (prefixes != config.end()) {
        number++;
        prefixes = config.find(::std::u16string(PREFIX_KEY) + u"." + ::inflection::util::StringUtils::to_u16string(number));
    }
    return number;
}

void SemiticWordAndDelimiterTokenExtractor::addPrefix(SemiticWordAndDelimiterTokenExtractor_AffixRecord* tree, ::std::u16string_view affix)
{
    auto lastAffixRecord = tree;
    inflection::util::Validate::isFalse(affix.empty());
    for (auto character : affix) {
        lastAffixRecord = npc(lastAffixRecord)->addNextCharacter(character);
    }
    npc(lastAffixRecord)->isFinal = true;
}

void SemiticWordAndDelimiterTokenExtractor::addPrefixes(const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    auto constexpr commaSeparator = u",";
    int32_t idx = 0;
    auto prefixes = config.find(::std::u16string(PREFIX_KEY) + u"." + ::inflection::util::StringUtils::to_u16string(idx));
    while (prefixes != config.end()) {
        ::std::u16string prefixesString;
        ::inflection::util::StringViewUtils::lowercase(&prefixesString, prefixes->second, getLocale());
        prefixTree[idx] = SemiticWordAndDelimiterTokenExtractor_AffixRecord();
        for (::inflection::util::DelimitedStringIterator iterator(prefixesString, commaSeparator); iterator.hasNext(); ++iterator) {
            addPrefix(&prefixTree[idx], *iterator);
        }
        idx++;
        prefixes = config.find(::std::u16string(PREFIX_KEY) + u"." + ::inflection::util::StringUtils::to_u16string(idx));
    }
}

void SemiticWordAndDelimiterTokenExtractor::addSuffixes(const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    auto constexpr commaSeparator = u",";
    auto suffixes = config.find(SUFFIX_KEY);
    if (suffixes != config.end()) {
        ::std::u16string suffixesString;
        ::inflection::util::StringViewUtils::lowercase(&suffixesString, suffixes->second, getLocale());
        for (::inflection::util::DelimitedStringIterator iterator(suffixesString, commaSeparator); iterator.hasNext(); ++iterator) {
            addSuffix(&suffixTree, *iterator);
        }
    }
}

void SemiticWordAndDelimiterTokenExtractor::addSuffix(SemiticWordAndDelimiterTokenExtractor_AffixRecord* tree, ::std::u16string_view affix)
{
    ::std::u16string affixCopy(affix);
    addPrefix(tree, *::inflection::util::StringUtils::reverse(&affixCopy));
}


} // namespace inflection::tokenizer
