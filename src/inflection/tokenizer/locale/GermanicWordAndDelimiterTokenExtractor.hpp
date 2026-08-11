/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <string_view>

class inflection::tokenizer::locale::GermanicWordAndDelimiterTokenExtractor final
    : public TokenExtractor
{

public:
    typedef TokenExtractor super;

public: /* protected */
    ::inflection::tokenizer::dictionary::GermanicDecompounder decompounder;

public: /* protected */
    iterator::TokenExtractorIterator* createIterator(std::u16string_view str) const override;

public:

    GermanicWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, const::inflection::tokenizer::dictionary::DictionaryTokenizerConfig& dictionaryTokenizerConfig);
private:
    GermanicWordAndDelimiterTokenExtractor(const GermanicWordAndDelimiterTokenExtractor& other) = delete;
    GermanicWordAndDelimiterTokenExtractor& operator=(const GermanicWordAndDelimiterTokenExtractor& other) = delete;
};
