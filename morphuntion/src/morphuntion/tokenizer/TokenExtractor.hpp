/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/tokenizer/iterator/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/tokenizer/TokenExtractor.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <icu4cxx/BreakIterator.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <string_view>

class morphuntion::tokenizer::TokenExtractor
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

protected: /* package */
    static constexpr char16_t NON_DECOMPOUND_FILE_KEY[] = { u"tokenizer.nonDecompound.file" };
    static constexpr char16_t DECOMPOUND_KEY[] = { u"tokenizer.decompound" };

private: /* package */
    ::morphuntion::util::ULocale locale;

protected: /* protected */
    ::std::set<::std::u16string_view>* const wordsToNotSplit { nullptr };

private:
    icu4cxx::RegularExpression *splitPattern { nullptr };

public:
    ::std::set<::std::u16string_view>* createWordsToNotSplit(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(::std::u16string& workspace, const ::morphuntion::util::ULocale& locale, std::u16string_view str)) const;
private:
    static bool isIndivisibleWordNormalized(::std::u16string& workspace, const ::morphuntion::util::ULocale& locale, std::u16string_view str);
public:
    virtual morphuntion::tokenizer::iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const;

public: /* protected */
    virtual ::std::u16string* normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view fullCharSequence, int32_t start) const;
    static ::std::u16string getRequiredKeyAsPath(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::u16string_view key);

public:
    const ::morphuntion::util::ULocale& getLocale() const;
    bool isContainRegex() const;

    TokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
    TokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::set<::std::u16string_view>* wordsToNotSplit);
    ~TokenExtractor() override;

private:
    TokenExtractor(const TokenExtractor& other) = delete;
    TokenExtractor& operator=(const TokenExtractor& other) = delete;

    friend class iterator::RegexSplitIterator;
};
