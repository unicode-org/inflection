/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#include <inflection/tokenizer/locale/DravidianWordAndDelimiterTokenExtractor.hpp>

#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/locale/DravidianTokenExtractorIterator.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::locale {

static constexpr char16_t SUFFIX_KEY[] = { u"tokenizer.suffixes" };

DravidianWordAndDelimiterTokenExtractor::DravidianWordAndDelimiterTokenExtractor(
    const ::inflection::util::ULocale& locale,
    const ::icu4cxx::UnicodeSet& nativeSet,
    const ::std::map<::std::u16string_view, const char16_t*>& config)
    : super(locale, config)
    , nativeSet(nativeSet)
{
    addSuffixes(config);
}

inflection::tokenizer::iterator::TokenExtractorIterator*
DravidianWordAndDelimiterTokenExtractor::createIterator(std::u16string_view str) const
{
    return new DravidianTokenExtractorIterator(*this, str);
}

bool DravidianWordAndDelimiterTokenExtractor::isKnownWord(std::u16string_view word) const
{
    return wordsToNotSplit != nullptr && npc(wordsToNotSplit)->find(word) != npc(wordsToNotSplit)->end();
}

void DravidianWordAndDelimiterTokenExtractor::decompoundWord(
    std::vector<int32_t>* boundaries,
    std::u16string_view charSequence,
    int32_t start,
    int32_t end) const
{
    std::u16string_view word = charSequence.substr(start, end - start);
    if (word.length() <= MINIMUM_WORD_LENGTH || !::inflection::util::UnicodeSetUtils::containsSome(nativeSet, word) || isKnownWord(word)) {
        return;
    }

    auto suffixIdx = int32_t(word.length());
    auto node = &suffixTree;
    auto lastGoodNode = node;
    int32_t remainingLength = suffixIdx;

    while (remainingLength > MINIMUM_WORD_LENGTH) {
        char16_t ch = word[--suffixIdx];
        node = npc(node)->getNextAffix(ch);
        if (!node)
            break;
        lastGoodNode = node;
        remainingLength--;
    }

    if (npc(lastGoodNode)->isFinal && remainingLength != int32_t(word.length())) {
        npc(boundaries)->emplace_back(start + remainingLength);
        npc(boundaries)->emplace_back(end);
    }
}

void DravidianWordAndDelimiterTokenExtractor::addSuffixes(const ::std::map<::std::u16string_view, const char16_t*>& config)
{
    static constexpr char16_t commaSeparator[] = { u',' };
    auto suffixes = config.find(SUFFIX_KEY);
    if (suffixes != config.end()) {
        ::std::u16string suffixString;
        ::inflection::util::StringViewUtils::lowercase(&suffixString, suffixes->second, getLocale());
        for (::inflection::util::DelimitedStringIterator iterator(suffixString, commaSeparator); iterator.hasNext(); ++iterator) {
            addSuffix(&suffixTree, *iterator);
        }
    }
}

void DravidianWordAndDelimiterTokenExtractor::addSuffix(AffixRecord* tree, ::std::u16string_view affix)
{
    ::std::u16string reversedAffix = *::inflection::util::StringUtils::reverse(&std::u16string(affix));
    auto node = tree;
    for (char16_t ch : reversedAffix) {
        node = npc(node)->addNextCharacter(ch);
    }
    node->isFinal = true;
}

} // namespace inflection::tokenizer::locale
