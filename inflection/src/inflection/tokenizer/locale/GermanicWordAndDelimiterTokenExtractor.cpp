/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/GermanicWordAndDelimiterTokenExtractor.hpp>

#include <inflection/tokenizer/locale/GermanicTokenExtractorIterator.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>
#include <memory>

namespace inflection::tokenizer::locale {

GermanicWordAndDelimiterTokenExtractor::GermanicWordAndDelimiterTokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, const dictionary::DictionaryTokenizerConfig& dictionaryTokenizerConfig)
    : super(locale, config)
    , decompounder(dictionaryTokenizerConfig, getRequiredKeyAsPath(locale, config, u"tokenizer.dictionary.file"))
{
}

iterator::TokenExtractorIterator* GermanicWordAndDelimiterTokenExtractor::createIterator(std::u16string_view str) const
{
    return new GermanicTokenExtractorIterator(decompounder, str);
}

} // namespace inflection::tokenizer
