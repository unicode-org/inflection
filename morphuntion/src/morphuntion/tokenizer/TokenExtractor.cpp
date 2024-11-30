/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/TokenExtractor.hpp>

#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/exception/IOException.hpp>
#include <morphuntion/exception/InvalidConfigurationException.hpp>
#include <morphuntion/resources/DataResource.hpp>
#include <morphuntion/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <morphuntion/tokenizer/StringTokenizerBridgeProvider.hpp>
#include <morphuntion/tokenizer/TokenUtil.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/ResourceLocator.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <sys/stat.h>
#include <memory>

namespace morphuntion::tokenizer {

TokenExtractor::TokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config)
    : TokenExtractor(locale, config, createWordsToNotSplit(locale, config, isIndivisibleWordNormalized))
{
}

TokenExtractor::TokenExtractor(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::set<::std::u16string_view>* wordsToNotSplit)
    : super()
    , locale(locale)
    , wordsToNotSplit(wordsToNotSplit)
{
    util::Validate::isTrue(wordsToNotSplit == nullptr || !wordsToNotSplit->empty());
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
    delete wordsToNotSplit;
    delete splitPattern;
}

constexpr char16_t TokenExtractor::NON_DECOMPOUND_FILE_KEY[];

constexpr char16_t TokenExtractor::DECOMPOUND_KEY[];

::std::set<::std::u16string_view>* TokenExtractor::createWordsToNotSplit(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, bool (*isIndivisibleWordNormalized)(::std::u16string& workspace, const ::morphuntion::util::ULocale& locale, std::u16string_view /*str*/)) const
{
    ::std::set<::std::u16string_view>* result = nullptr;
    auto nonDecompoundFile = config.find(NON_DECOMPOUND_FILE_KEY);
    if (nonDecompoundFile != config.end() && nonDecompoundFile->second[0] != 0) {
        ::std::u16string nonDecompoundFileStr(nonDecompoundFile->second);
        if (::morphuntion::util::LoggerConfig::isDebugEnabled()) {
            ::morphuntion::util::Logger::debug(u"Loading non-decompound list from " + nonDecompoundFileStr);
        }
        try {
            ::std::u16string workspace;
            result = new ::std::set<::std::u16string_view>();
            auto in = ::morphuntion::resources::DataResource::getString(nonDecompoundFileStr);
            for (::morphuntion::util::DelimitedStringIterator iterator(in, u"\n"); iterator.hasNext(); ++iterator) {
                auto str = ::morphuntion::util::StringViewUtils::trim(*iterator);
                if (!str.empty() && str[0] != u'#') {
                    if (!isIndivisibleWordNormalized(workspace, locale, str)) {
                        throw morphuntion::exception::InvalidConfigurationException(std::u16string(u"String should already be normalized: ") + std::u16string(str));
                    }
                    npc(result)->insert(str);
                }
            }
        }
        catch (const ::morphuntion::exception::IOException&) {
            ::morphuntion::util::Logger::error(u"Error reading " + nonDecompoundFileStr);
            throw;
        }
    }
    return result;
}

bool TokenExtractor::isIndivisibleWordNormalized(::std::u16string& workspace, const ::morphuntion::util::ULocale& locale, std::u16string_view str)
{
    return *::morphuntion::util::StringViewUtils::lowercase(&workspace, str, locale) == str;
}

morphuntion::tokenizer::iterator::TokenExtractorIterator* TokenExtractor::createIterator(std::u16string_view str) const
{
    return new morphuntion::tokenizer::iterator::ICUTokenExtractorIterator(str.data(), int32_t(str.length()));
}

::std::u16string* TokenExtractor::normalize(::std::u16string* dest, std::u16string_view src, std::u16string_view, int32_t) const
{
    return ::morphuntion::util::StringViewUtils::lowercase(dest, src, locale);
}

::std::u16string TokenExtractor::getRequiredKeyAsPath(const ::morphuntion::util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config, ::std::u16string_view key)
{
    auto dictionaryEntry = config.find(key);
    if (dictionaryEntry == config.end() || dictionaryEntry->second == nullptr || dictionaryEntry->second[0] == 0) {
        throw ::morphuntion::exception::InvalidConfigurationException(::std::u16string(key) + u" should be defined");
    }
    ::std::u16string path(morphuntion::util::ResourceLocator::getRootForLocale(locale) + ::std::u16string(StringTokenizerBridgeProvider::TOKENIZER_DATA_DIR) + dictionaryEntry->second);
    ::std::string pathCStr;
    morphuntion::util::StringViewUtils::convert(&pathCStr, path);
    struct stat info;
    if (stat(pathCStr.c_str(), &info) != 0) {
        ::morphuntion::util::Logger::error(u"Path does not exist " + path);
    }
    return path;
}

const morphuntion::util::ULocale& TokenExtractor::getLocale() const
{
    return locale;
}

bool TokenExtractor::isContainRegex() const
{
    return splitPattern != nullptr;
}

} // namespace morphuntion::tokenizer
