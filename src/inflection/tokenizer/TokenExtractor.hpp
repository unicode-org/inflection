/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/iterator/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/Object.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <map>
#include <string>
#include <string_view>
#include <vector>

class inflection::tokenizer::TokenExtractor
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private: /* package */
    ::inflection::util::ULocale locale;
    ::std::vector<::std::u16string_view> wordsToNotSplit {  };
    icu4cxx::RegularExpression *splitPattern { nullptr };
    int32_t minLenWordsToSplit {  };
    int32_t maxLenWordsToSplit {  };

protected:
    void readWordsToNotSplit(const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(std::u16string_view str));
public:
    bool isWordToNotSplit(std::u16string_view str) const;
    virtual inflection::tokenizer::iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const;

public: /* protected */
    virtual ::std::u16string* normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view fullCharSequence, int32_t start) const;
    static ::std::u16string getRequiredKeyAsPath(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::u16string_view key);

public:
    const ::inflection::util::ULocale& getLocale() const;
    bool isContainRegex() const;

    TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
    TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(std::u16string_view str));
    ~TokenExtractor() override;

private:
    TokenExtractor(const TokenExtractor& other) = delete;
    TokenExtractor& operator=(const TokenExtractor& other) = delete;

    friend class iterator::RegexSplitIterator;
};
