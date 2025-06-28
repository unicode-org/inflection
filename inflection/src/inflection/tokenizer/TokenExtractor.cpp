/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/TokenExtractor.hpp>

#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/StringTokenizerBridgeProvider.hpp>
#include <inflection/tokenizer/TokenUtil.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <sys/stat.h>
#include <memory>

namespace inflection::tokenizer {

TokenExtractor::TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
    : TokenExtractor(locale, config, createWordsToNotSplit(locale, config, isIndivisibleWordNormalized))
{
}

TokenExtractor::TokenExtractor(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::set<::std::u16string_view>* wordsToNotSplit)
    : super()
    , locale(locale)
    , wordsToNotSplit(wordsToNotSplit)
{
    util::Validate::isTrue(wordsToNotSplit == nullptr || !wordsToNotSplit->empty());
    auto decompoundEntry = config.find(DECOMPOUND_KEY);
    if (decompoundEntry != config.end() && decompoundEntry->second[0] != 0) {
        ::std::u16string regex(u"^(" + ::std::u16string(decompoundEntry->second) + u")$");
        splitPattern.reset(new ::icu4cxx::RegularExpression(regex, UREGEX_CASE_INSENSITIVE, nullptr));
    }

    // Initialize the caches so that it doesn't possibly fail later.
    TokenUtil::isSignificant(u"A");
}

TokenExtractor::~TokenExtractor()
{
}

constexpr char16_t TokenExtractor::NON_DECOMPOUND_FILE_KEY[];

constexpr char16_t TokenExtractor::DECOMPOUND_KEY[];

::std::set<::std::u16string_view>* TokenExtractor::createWordsToNotSplit(const ::inflection::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(::std::u16string& workspace, const ::inflection::util::ULocale& locale, std::u16string_view /*str*/)) const
{
    ::std::set<::std::u16string_view>* result = nullptr;
    auto nonDecompoundFile = config.find(NON_DECOMPOUND_FILE_KEY);
    if (nonDecompoundFile != config.end() && nonDecompoundFile->second[0] != 0) {
        ::std::u16string nonDecompoundFileStr(nonDecompoundFile->second);
        if (::inflection::util::LoggerConfig::isDebugEnabled()) {
            ::inflection::util::Logger::debug(u"Loading non-decompound list from " + nonDecompoundFileStr);
        }
        try {
            ::std::u16string workspace;
            result = new ::std::set<::std::u16string_view>();
            auto in = ::inflection::resources::DataResource::getString(nonDecompoundFileStr);
            for (::inflection::util::DelimitedStringIterator iterator(in, u"\n"); iterator.hasNext(); ++iterator) {
                auto str = ::inflection::util::StringViewUtils::trim(*iterator);
                if (!str.empty() && str[0] != u'#') {
                    if (!isIndivisibleWordNormalized(workspace, locale, str)) {
                        throw inflection::exception::InvalidConfigurationException(std::u16string(u"String should already be normalized: ") + std::u16string(str));
                    }
                    npc(result)->insert(str);
                }
            }
        }
        catch (const ::inflection::exception::IOException&) {
            ::inflection::util::Logger::error(u"Error reading " + nonDecompoundFileStr);
            throw;
        }
    }
    return result;
}

bool TokenExtractor::isIndivisibleWordNormalized(::std::u16string& workspace, const ::inflection::util::ULocale& locale, std::u16string_view str)
{
    return *::inflection::util::StringViewUtils::lowercase(&workspace, str, locale) == str;
}

inflection::tokenizer::iterator::TokenExtractorIterator* TokenExtractor::createIterator(std::u16string_view str) const
{
    return new inflection::tokenizer::iterator::ICUTokenExtractorIterator(str.data(), int32_t(str.length()));
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
    ::std::string pathCStr;
    inflection::util::StringViewUtils::convert(&pathCStr, path);
    struct stat info;
    if (stat(pathCStr.c_str(), &info) != 0) {
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
