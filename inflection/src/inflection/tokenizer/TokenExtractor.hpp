/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/iterator/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/Object.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <icu4cxx/BreakIterator.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <string_view>

class inflection::tokenizer::TokenExtractor
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

protected: /* package */
    static constexpr char16_t NON_DECOMPOUND_FILE_KEY[] = { u"tokenizer.nonDecompound.file" };
    static constexpr char16_t DECOMPOUND_KEY[] = { u"tokenizer.decompound" };

private: /* package */
    ::inflection::util::ULocale locale;

protected: /* protected */
    std::unique_ptr<::std::set<::std::u16string_view>> const wordsToNotSplit { nullptr };

private:
    std::unique_ptr<icu4cxx::RegularExpression> splitPattern { nullptr };

public:
    ::std::set<::std::u16string_view>* createWordsToNotSplit(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(::std::u16string& workspace, const ::inflection::util::ULocale& locale, std::u16string_view str)) const;
private:
    static bool isIndivisibleWordNormalized(::std::u16string& workspace, const ::inflection::util::ULocale& locale, std::u16string_view str);
public:
    virtual inflection::tokenizer::iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const;

public: /* protected */
    virtual ::std::u16string* normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view fullCharSequence, int32_t start) const;
    static ::std::u16string getRequiredKeyAsPath(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::u16string_view key);

public:
    const ::inflection::util::ULocale& getLocale() const;
    bool isContainRegex() const;

    TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
    TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::set<::std::u16string_view>* wordsToNotSplit);
    ~TokenExtractor() override;

private:
    TokenExtractor(const TokenExtractor& other) = delete;
    TokenExtractor& operator=(const TokenExtractor& other) = delete;

    friend class iterator::RegexSplitIterator;
};
