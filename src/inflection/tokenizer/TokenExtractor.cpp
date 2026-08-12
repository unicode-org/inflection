/*
 * Copyright 2018-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/TokenExtractor.hpp>

#include <inflection/exception/IOException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/StringTokenizerBridgeProvider.hpp>
#include <inflection/tokenizer/TokenUtil.hpp>
#include <inflection/util/AutoFileDescriptor.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <algorithm>
#include <memory>

namespace inflection::tokenizer {

static constexpr const char16_t* NON_DECOMPOUND_FILE_KEY = u"tokenizer.nonDecompound.file";
static constexpr const char16_t* DECOMPOUND_KEY = u"tokenizer.decompound";

TokenExtractor::TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
    : TokenExtractor(locale, config, inflection::util::StringViewUtils::isAllLowerCase)
{
}

TokenExtractor::TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(std::u16string_view str))
    : super()
    , locale(locale)
{
    readWordsToNotSplit(config, isIndivisibleWordNormalized);

    auto decompoundEntry = config.find(DECOMPOUND_KEY);
    if (decompoundEntry != config.end() && decompoundEntry->second[0] != 0) {
        ::std::u16string regex(u"^(" + ::std::u16string(decompoundEntry->second) + u")$");
        splitPattern = new ::icu4cxx::RegularExpression(regex, UREGEX_CASE_INSENSITIVE, nullptr);
    }

    // Initialize the caches so that it doesn't possibly fail later.
    TokenUtil::isSignificant(u"A");
}

TokenExtractor::~TokenExtractor()
{
    delete splitPattern;
}

void TokenExtractor::readWordsToNotSplit(const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(std::u16string_view /*str*/))
{
    auto nonDecompoundFile = config.find(NON_DECOMPOUND_FILE_KEY);
    if (nonDecompoundFile != config.end() && nonDecompoundFile->second[0] != 0) {
        ::std::u16string_view nonDecompoundFileStr(nonDecompoundFile->second);
        try {
            std::u16string_view in(::inflection::resources::DataResource::getString(nonDecompoundFileStr));
            std::u16string_view lastWord;
            int32_t count = 0;
            minLenWordsToSplit = INT32_MAX;
            for (::inflection::util::DelimitedStringIterator iterator(in, u"\n"); iterator.hasNext(); ++iterator) {
                auto str = ::inflection::util::StringViewUtils::trim(*iterator);
                if (!str.empty() && str.front() != u'#') {
                    if (!isIndivisibleWordNormalized(str)) {
                        throw inflection::exception::InvalidConfigurationException(std::u16string(u"String should already be normalized: ") + std::u16string(str));
                    }
                    if (!lastWord.empty() && lastWord >= str) {
                        throw inflection::exception::InvalidConfigurationException(std::u16string(u"Strings should be sorted: ") + std::u16string(str));
                    }
                    lastWord = str;
                    int32_t strLen = static_cast<int32_t>(str.length());
                    minLenWordsToSplit = std::min(minLenWordsToSplit, strLen);
                    maxLenWordsToSplit = std::max(maxLenWordsToSplit, strLen);
                    count++;
                }
            }
            wordsToNotSplit.reserve(count);
            for (::inflection::util::DelimitedStringIterator iterator(in, u"\n"); iterator.hasNext(); ++iterator) {
                auto str = ::inflection::util::StringViewUtils::trim(*iterator);
                if (!str.empty() && str.front() != u'#') {
                    wordsToNotSplit.emplace_back(str);
                }
            }
        }
        catch (const ::inflection::exception::IOException&) {
            ::inflection::util::Logger::error(u"Error reading " + std::u16string(nonDecompoundFileStr));
            throw;
        }
    }
}

bool TokenExtractor::isWordToNotSplit(std::u16string_view str) const {
    if (minLenWordsToSplit == 0) {
        return false;
    }
    const auto strLen = static_cast<int32_t>(str.length());
    return minLenWordsToSplit <= strLen && strLen <= maxLenWordsToSplit
        && std::ranges::binary_search(wordsToNotSplit, str,
                                      [](auto str1, auto str2) {return str1 < str2;});
}

inflection::tokenizer::iterator::TokenExtractorIterator* TokenExtractor::createIterator(std::u16string_view str) const
{
    return new inflection::tokenizer::iterator::ICUTokenExtractorIterator(str.data(), static_cast<int32_t>(str.length()));
}

::std::u16string* TokenExtractor::normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view, int32_t) const
{
    return ::inflection::util::StringViewUtils::lowercase(dest, src, locale);
}

::std::u16string TokenExtractor::getRequiredKeyAsPath(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::u16string_view key)
{
    auto dictionaryEntry = config.find(key);
    if (dictionaryEntry == config.end() || dictionaryEntry->second == nullptr || dictionaryEntry->second[0] == 0) {
        throw ::inflection::exception::InvalidConfigurationException(::std::u16string(key) + u" should be defined");
    }
    ::std::u16string path(inflection::util::ResourceLocator::getRootForLocale(locale) + ::std::u16string(StringTokenizerBridgeProvider::TOKENIZER_DATA_DIR) + dictionaryEntry->second);
    if (!inflection::util::AutoFileDescriptor::isAccessibleFile(path)) {
        ::inflection::util::Logger::error(u"Path does not exist " + path);
    }
    return path;
}

const inflection::util::ULocale& TokenExtractor::getLocale() const
{
    return locale;
}

bool TokenExtractor::isContainRegex() const
{
    return splitPattern != nullptr;
}

} // namespace inflection::tokenizer
