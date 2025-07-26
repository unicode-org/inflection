/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/TokenizerFactory.hpp>

#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/MissingResourceException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/npc.hpp>
#include <unicode/ustring.h>
#include <mutex>
#include <numeric>

#include <inflection/tokenizer/locale/DefaultTokenizer.hpp>
#include <inflection/tokenizer/locale/ar/ArTokenizer.hpp>
#include <inflection/tokenizer/locale/da/DaTokenizer.hpp>
#include <inflection/tokenizer/locale/nb/NbTokenizer.hpp>
#include <inflection/tokenizer/locale/nl/NlTokenizer.hpp>
#include <inflection/tokenizer/locale/sv/SvTokenizer.hpp>

namespace inflection::tokenizer {

static const char16_t CONFIG_RESOURCE_PREFIX[] = u"/org/unicode/inflection/tokenizer/config_";
static const char16_t PROPERTIES_SUFFIX[] = u".properties";
static const char16_t TOKENIZER_CLASS[] = u"tokenizer.implementation.class";
static const char16_t TOKENIZER_LANGUAGE_ALIAS[] = u"tokenizer.alias.language";
static constexpr auto COMPONENT_TOKENIZER = u"Tokenizer";

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

::std::map<::std::string, const Tokenizer*, std::less<>>* TOKENIZER_CACHE()
{
    static auto TOKENIZER_CACHE_ = new ::std::map<::std::string, const Tokenizer*, std::less<>>();
    return TOKENIZER_CACHE_;
}

bool readConfig(const ::std::u16string& locale, ::std::map<::std::u16string_view, const char16_t*>* config)
{
    *npc(config) = resources::DataResource::getProperties(CONFIG_RESOURCE_PREFIX + locale + PROPERTIES_SUFFIX, true);
    return !npc(config)->empty();
}

Tokenizer* TokenizerFactory::createTokenizer(const util::ULocale& locale)
{
    const Tokenizer* tokenizer = nullptr;
    auto tokenizerCache = TOKENIZER_CACHE();
    {
        std::lock_guard<std::mutex> guard(CLASS_MUTEX());
        auto tokenizerEntry = npc(tokenizerCache)->find(locale.getName());
        if (tokenizerEntry != npc(tokenizerCache)->end()) {
            tokenizer = tokenizerEntry->second;
        }
    }
    if (tokenizer == nullptr) {
        ::std::map<::std::u16string_view, const char16_t*> config;
        util::ULocale fallbackLocale(locale);
        while (!fallbackLocale.getName().empty()) {
            if (readConfig(fallbackLocale.toString(), &config)) {
                auto aliasLocale = config.find(TOKENIZER_LANGUAGE_ALIAS);
                if (aliasLocale != config.end()) {
                    readConfig(aliasLocale->second, &config);
                }
                break;
            }
            fallbackLocale = fallbackLocale.getFallback();
        }
        if (config.empty()) {
            if (util::LoggerConfig::isWarnEnabled()) {
                util::Logger::warn(u"The tokenizer for " + locale.toString() + u" is unknown. The default will be used.");
            }
            if (!readConfig(util::StringViewUtils::to_u16string(util::LocaleUtils::ENGLISH().getLanguage()), &config)) {
                // The world is missing!
                throw exception::MissingResourceException(u"The tokenizer configuration files are missing.", u"TokenizerFactory", CONFIG_RESOURCE_PREFIX);
            }
        }
        {
            std::lock_guard<std::mutex> guard(CLASS_MUTEX());
            auto tokenizerEntry = npc(tokenizerCache)->find(fallbackLocale.getName());
            if (tokenizerEntry != npc(tokenizerCache)->end()) {
                // Reuse the fallback so that we don't grow the cache indefinitely with variations.
                tokenizer = tokenizerEntry->second;
                npc(tokenizerCache)->emplace(locale.getName(), tokenizer);
            }
            else {
                tokenizerEntry = npc(tokenizerCache)->find(locale.getName());
                if (tokenizerEntry == npc(tokenizerCache)->end()) {
                    // Neither the fallback nor the requested locale have already created a tokenizer. Cache a new tokenizer.
                    tokenizer = createTokenizerObject(locale, config);
                    npc(tokenizerCache)->emplace(fallbackLocale.getName(), tokenizer);
                    npc(tokenizerCache)->emplace(locale.getName(), tokenizer);
                }
                else {
                    // This should be impossible! The fallback should have been reused. We take the safe route just in case.
                    tokenizer = tokenizerEntry->second;
                }
            }
        }
    }
    return new Tokenizer(npc(tokenizer)->tokenExtractor);
}

Tokenizer* TokenizerFactory::createTokenizerObject(const util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& systemConfig)
{
    auto inClassNameEntry = systemConfig.find(TOKENIZER_CLASS);
    if (inClassNameEntry != systemConfig.end()) {
        auto inClassName = inClassNameEntry->second;
        static const struct {
            const char16_t *className;
            tokenizer::TokenExtractor* (*construct)(const util::ULocale& locale, const ::std::map<::std::u16string_view, const char16_t*>& config);
        }
        TOKENIZER_CONSTRUCTORS[] = {
            // It's very important to keep this in sorted order for the binary search
            {u"DefaultTokenizer", &tokenizer::locale::DefaultTokenizer::createTokenExtractor},
            {u"locale.ar.ArTokenizer", &tokenizer::locale::ar::ArTokenizer::createTokenExtractor},
            {u"locale.da.DaTokenizer", &tokenizer::locale::da::DaTokenizer::createTokenExtractor},
            {u"locale.nb.NbTokenizer", &tokenizer::locale::nb::NbTokenizer::createTokenExtractor},
            {u"locale.nl.NlTokenizer", &tokenizer::locale::nl::NlTokenizer::createTokenExtractor},
            {u"locale.sv.SvTokenizer", &tokenizer::locale::sv::SvTokenizer::createTokenExtractor},
        };

        int32_t start = 0;
        int32_t limit = std::ssize(TOKENIZER_CONSTRUCTORS) - 1;
        while (start <= limit) {
            int32_t mid = std::midpoint(start, limit);
            int32_t comp = u_strcmp((const UChar*)inClassName, (const UChar*)TOKENIZER_CONSTRUCTORS[mid].className);
            if (comp < 0) {
                limit = mid - 1;
            }
            else if (comp > 0) {
                start = mid + 1;
            }
            else {
                if (util::LoggerConfig::isInfoEnabled()) {
                    // We check for the logging status so that we don't construct a string that is never used.
                    util::Logger::infoComponent(COMPONENT_TOKENIZER, u"The tokenizer for " + locale.toString() + u" is being constructed for the first time.");
                }
                auto tokenizer = new Tokenizer(TOKENIZER_CONSTRUCTORS[mid].construct(locale, systemConfig));
                // Initialize the data loading and caches so that it doesn't possibly fail later.
                delete npc(tokenizer)->createTokenChain(u"initialization");
                return tokenizer;
            }
        }
    }
    util::Logger::error(u"Error creating tokenizer for locale " + locale.toString());
    throw exception::InvalidConfigurationException(u"Error creating tokenizer for locale " + locale.toString());
}

} // namespace inflection
