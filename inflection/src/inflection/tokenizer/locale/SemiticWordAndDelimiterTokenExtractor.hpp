/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <inflection/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor_AffixRecord.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <vector>
#include <string_view>

class inflection::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor final
    : public TokenExtractor
{
public:
    typedef TokenExtractor super;

private:
    const ::icu4cxx::UnicodeSet nativeSet;
    SemiticWordAndDelimiterTokenExtractor_AffixRecord suffixTree {  };
    ::std::vector<SemiticWordAndDelimiterTokenExtractor_AffixRecord> prefixTree {  };
    ::std::u16string_view toNormalize {  };
    ::std::u16string_view replacement {  };

public:
    inflection::tokenizer::iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const override;

private:
    bool isKnownWord(std::u16string_view compoundWord) const;

public: /* protected */
    void decompoundWord(std::vector<int32_t>* boundaries, std::u16string_view phrase, int32_t start, int32_t end) const;
    ::std::u16string* normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view fullCharSequence, int32_t start) const override;

private:
    static int32_t getPrefixClasses(const ::std::map<::std::u16string_view, const char16_t*>& config);
    static void addPrefix(SemiticWordAndDelimiterTokenExtractor_AffixRecord* tree, ::std::u16string_view affix);
    void addPrefixes(const ::std::map<::std::u16string_view, const char16_t*>& config);
    void addSuffixes(const ::std::map<::std::u16string_view, const char16_t*>& config);
    static void addSuffix(SemiticWordAndDelimiterTokenExtractor_AffixRecord* tree, ::std::u16string_view affix);


public:
    SemiticWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::icu4cxx::UnicodeSet& nativeSet, const ::std::map<::std::u16string_view, const char16_t*>& config);

    static constexpr int32_t MINIMUM_WORD_LENGTH { 2 };
};
