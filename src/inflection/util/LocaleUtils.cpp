/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LocaleUtils.hpp>

#include <inflection/util/LocaleConstants.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/unum.h>
#include <unicode/uscript.h>
#include <cstring>
#include <numeric>

#define DEFINE_LOCALE1(NAME) \
const ULocale& LocaleUtils::NAME() {\
    static auto NAME##_ = new ULocale(LocaleConstants::NAME);\
    return *NAME##_;\
}

#define DEFINE_LOCALE(NAME, CONSTANT, SUBTAG) \
const ULocale& LocaleUtils::NAME() {\
    static auto NAME##_ = new ULocale(CONSTANT, SUBTAG);\
    return *NAME##_;\
}

namespace inflection::util {

DEFINE_LOCALE1(ARABIC)
DEFINE_LOCALE(SAUDI_ARABIA, LocaleConstants::ARABIC, "SA")

DEFINE_LOCALE1(BANGLA)
DEFINE_LOCALE(INDIA_BANGLA, LocaleConstants::BANGLA, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(BULGARIAN)
DEFINE_LOCALE(BULGARIA, LocaleConstants::BULGARIAN, "BG")

DEFINE_LOCALE1(CATALAN)
DEFINE_LOCALE(SPAIN_CATALAN, LocaleConstants::CATALAN, "ES")

DEFINE_LOCALE1(CZECH)
DEFINE_LOCALE(CZECHIA, LocaleConstants::CZECH, "CZ")

DEFINE_LOCALE1(DANISH)
DEFINE_LOCALE(DENMARK, LocaleConstants::DANISH, "DK")

DEFINE_LOCALE1(GERMAN)
DEFINE_LOCALE(GERMANY, LocaleConstants::GERMAN, "DE")
DEFINE_LOCALE(SWITZERLAND_GERMAN, LocaleConstants::GERMAN, LocaleConstants::REGION_SWITZERLAND)
DEFINE_LOCALE(AUSTRIA, LocaleConstants::GERMAN, LocaleConstants::REGION_AUSTRIA)

DEFINE_LOCALE1(GREEK)
DEFINE_LOCALE(GREECE, LocaleConstants::GREEK, "GR")

DEFINE_LOCALE1(ENGLISH)
DEFINE_LOCALE(US, LocaleConstants::ENGLISH, "US")
DEFINE_LOCALE(AUSTRALIA_ENGLISH, LocaleConstants::ENGLISH, "AU")
DEFINE_LOCALE(CANADIAN_ENGLISH, LocaleConstants::ENGLISH, "CA")
DEFINE_LOCALE(SINGAPOREAN_ENGLISH, LocaleConstants::ENGLISH, "SG")
DEFINE_LOCALE(GREAT_BRITAIN, LocaleConstants::ENGLISH, "GB")
DEFINE_LOCALE(NEW_ZEALAND_ENGLISH, LocaleConstants::ENGLISH, "NZ")
DEFINE_LOCALE(INDIA_ENGLISH, LocaleConstants::ENGLISH, LocaleConstants::REGION_INDIA)
DEFINE_LOCALE(SOUTH_AFRICAN_ENGLISH, LocaleConstants::ENGLISH, "ZA")
DEFINE_LOCALE(IRISH_ENGLISH, LocaleConstants::ENGLISH, "IE")

DEFINE_LOCALE1(SPANISH)
DEFINE_LOCALE(SPAIN_SPANISH, LocaleConstants::SPANISH, "ES")
DEFINE_LOCALE(MEXICO_SPANISH, LocaleConstants::SPANISH, "MX")
DEFINE_LOCALE(US_SPANISH, LocaleConstants::SPANISH, "US")
DEFINE_LOCALE(CHILEAN_SPANISH, LocaleConstants::SPANISH, "CL")

DEFINE_LOCALE1(ESTONIAN)
DEFINE_LOCALE(ESTONIA, LocaleConstants::ESTONIAN, "EE")

DEFINE_LOCALE1(FINNISH)
DEFINE_LOCALE(FINLAND, LocaleConstants::FINNISH, "FI")

DEFINE_LOCALE1(FRENCH)
DEFINE_LOCALE(FRANCE, LocaleConstants::FRENCH, "FR")
DEFINE_LOCALE(BELGIUM_FRENCH, LocaleConstants::FRENCH, LocaleConstants::REGION_BELGIUM)
DEFINE_LOCALE(SWITZERLAND_FRENCH, LocaleConstants::FRENCH, LocaleConstants::REGION_SWITZERLAND)
DEFINE_LOCALE(CANADIAN_FRENCH, LocaleConstants::FRENCH, "CA")

DEFINE_LOCALE1(GUJARATI)
DEFINE_LOCALE(INDIA_GUJARATI, LocaleConstants::GUJARATI, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(HEBREW)
DEFINE_LOCALE(ISRAEL, LocaleConstants::HEBREW, "IL")

DEFINE_LOCALE1(HINDI)
DEFINE_LOCALE(INDIA_HINDI, LocaleConstants::HINDI, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(CROATIAN)
DEFINE_LOCALE(CROATIA, LocaleConstants::CROATIAN, "HR")

DEFINE_LOCALE1(HUNGARIAN)
DEFINE_LOCALE(HUNGARY, LocaleConstants::HUNGARIAN, "HU")

DEFINE_LOCALE1(INDONESIAN)
DEFINE_LOCALE(INDONESIA, LocaleConstants::INDONESIAN, "ID")

DEFINE_LOCALE1(ICELANDIC)
DEFINE_LOCALE(ICELAND, LocaleConstants::ICELANDIC, "IS")

DEFINE_LOCALE1(ITALIAN)
DEFINE_LOCALE(ITALY, LocaleConstants::ITALIAN, "IT")
DEFINE_LOCALE(SWITZERLAND_ITALIAN, LocaleConstants::ITALIAN, LocaleConstants::REGION_SWITZERLAND)

DEFINE_LOCALE1(JAPANESE)
DEFINE_LOCALE(JAPAN, LocaleConstants::JAPANESE, "JP")

DEFINE_LOCALE1(KAZAKH)
DEFINE_LOCALE(KAZAKHSTAN, LocaleConstants::KAZAKH, "KZ")

DEFINE_LOCALE1(KANNADA)
DEFINE_LOCALE(INDIA_KANNADA, LocaleConstants::KANNADA, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(KOREAN)
DEFINE_LOCALE(KOREA, LocaleConstants::KOREAN, "KR")

DEFINE_LOCALE1(LITHUANIAN)
DEFINE_LOCALE(LITHUANIA, LocaleConstants::LITHUANIAN, "LT")

DEFINE_LOCALE1(MALAYALAM)
DEFINE_LOCALE(INDIA_MALAYALAM, LocaleConstants::MALAYALAM, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(MARATHI)
DEFINE_LOCALE(INDIA_MARATHI, LocaleConstants::MARATHI, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(MALAY)
DEFINE_LOCALE(MALAYSIA, LocaleConstants::MALAY, "MY")

DEFINE_LOCALE1(NORWEGIAN)
DEFINE_LOCALE(NORWAY, LocaleConstants::NORWEGIAN, "NO")

DEFINE_LOCALE1(DUTCH)
DEFINE_LOCALE(NETHERLANDS, LocaleConstants::DUTCH, "NL")
DEFINE_LOCALE(BELGIUM_DUTCH, LocaleConstants::DUTCH, LocaleConstants::REGION_BELGIUM)

DEFINE_LOCALE1(ODIA)
DEFINE_LOCALE(INDIA_ODIA, LocaleConstants::ODIA, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(PUNJABI)
DEFINE_LOCALE(INDIA_PUNJABI, LocaleConstants::PUNJABI, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(POLISH)
DEFINE_LOCALE(POLAND, LocaleConstants::POLISH, "PL")

DEFINE_LOCALE1(PORTUGUESE)
DEFINE_LOCALE(PORTUGAL, LocaleConstants::PORTUGUESE, "PT")
DEFINE_LOCALE(BRAZIL, LocaleConstants::PORTUGUESE, "BR")

DEFINE_LOCALE1(ROMANIAN)
DEFINE_LOCALE(ROMANIA, LocaleConstants::ROMANIAN, "RO")

DEFINE_LOCALE1(RUSSIAN)
DEFINE_LOCALE(RUSSIA, LocaleConstants::RUSSIAN, "RU")

DEFINE_LOCALE1(SERBIAN)
DEFINE_LOCALE(SERBIA, LocaleConstants::SERBIAN, "RS")

DEFINE_LOCALE1(SLOVAK)
DEFINE_LOCALE(SLOVAKIA, LocaleConstants::SLOVAK, "SK")

DEFINE_LOCALE1(SWEDISH)
DEFINE_LOCALE(SWEDEN, LocaleConstants::SWEDISH, "SE")

DEFINE_LOCALE1(TAMIL)
DEFINE_LOCALE(INDIA_TAMIL, LocaleConstants::TAMIL, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(TELUGU)
DEFINE_LOCALE(INDIA_TELUGU, LocaleConstants::TELUGU, LocaleConstants::REGION_INDIA)

DEFINE_LOCALE1(THAI)
DEFINE_LOCALE(THAILAND, LocaleConstants::THAI, "TH")

DEFINE_LOCALE1(TURKISH)
DEFINE_LOCALE(TURKEY, LocaleConstants::TURKISH, "TR")

DEFINE_LOCALE1(UKRAINIAN)
DEFINE_LOCALE(UKRAINE, LocaleConstants::UKRAINIAN, "UA")

DEFINE_LOCALE1(URDU)
DEFINE_LOCALE(INDIA_URDU, LocaleConstants::URDU, LocaleConstants::REGION_INDIA)
DEFINE_LOCALE(PAKISTAN_URDU, LocaleConstants::URDU, LocaleConstants::REGION_PAKISTAN)

DEFINE_LOCALE1(VIETNAMESE)
DEFINE_LOCALE(VIETNAM, LocaleConstants::VIETNAMESE, "VN")

DEFINE_LOCALE1(SHANGHAINESE)
DEFINE_LOCALE(CHINA_SHANGHAINESE, LocaleConstants::SHANGHAINESE, LocaleConstants::REGION_CHINA)

DEFINE_LOCALE1(CANTONESE)
DEFINE_LOCALE(CHINA_CANTONESE, LocaleConstants::CANTONESE, LocaleConstants::REGION_CHINA)
DEFINE_LOCALE(HONGKONG_CANTONESE, LocaleConstants::CANTONESE, LocaleConstants::REGION_HONGKONG)

DEFINE_LOCALE1(CHINESE)
DEFINE_LOCALE(SIMPLIFIED_CHINESE, LocaleConstants::CHINESE, LocaleConstants::SCRIPT_SIMPLIFIED_HAN)
DEFINE_LOCALE(TRADITIONAL_CHINESE, LocaleConstants::CHINESE, LocaleConstants::SCRIPT_TRADITIONAL_HAN)
DEFINE_LOCALE(HONGKONG_CHINESE, LocaleConstants::CHINESE, LocaleConstants::REGION_HONGKONG)
DEFINE_LOCALE(CHINA, LocaleConstants::CHINESE, LocaleConstants::REGION_CHINA)
DEFINE_LOCALE(TAIWAN, LocaleConstants::CHINESE, LocaleConstants::REGION_TAIWAN)

DEFINE_LOCALE1(ROOT)

static std::map<::std::string, ::std::vector<ULocale>, std::less<>>* createSupportedLocaleMapToBase(const ::std::map<::std::u16string_view, const char16_t*>& config, std::u16string_view group)
{
    auto supportedLocales = new std::map< ::std::string, ::std::vector<ULocale>, std::less<>>();
    ::std::string language;
    for (const auto& [key, values] : config) {
        StringViewUtils::convert(&language, key.substr(group.length() + 1));
        npc(supportedLocales)->emplace(language, LocaleUtils::toLocales(values));
    }
    return supportedLocales;
}

static constexpr char16_t LOCALE_SUPPORT_GROUP_[] = u"locale.group";

const ::std::map<::std::string, ::std::vector<ULocale>, std::less<>>& LocaleUtils::BASE_SUPPORTED_LOCALES()
{
    static auto BASE_SUPPORTED_LOCALES_ = createSupportedLocaleMapToBase(inflection::resources::DataResource::getProperties(u"/org/unicode/inflection/locale/supported-locales.properties"), LOCALE_SUPPORT_GROUP_);
    return *npc(BASE_SUPPORTED_LOCALES_);
}

ULocale LocaleUtils::toLocale(const std::u16string &localeString) {
    if (localeString.empty()) {
        return ROOT();
    }
    ::std::string localeStr;
    StringViewUtils::convert(&localeStr, localeString);
    return ULocale(localeStr);
}

::std::vector<ULocale> LocaleUtils::toLocales(const std::u16string &localesString)
{
    ::std::vector<ULocale> locales;
    for (DelimitedStringIterator iterator(localesString, u","); iterator.hasNext(); ++iterator) {
        auto localeStr = StringViewUtils::trim(*iterator);
        if (!localeStr.empty()) {
            locales.push_back(toLocale(::std::u16string(localeStr)));
        }
    }
    return locales;
}

::std::u16string LocaleUtils::toString(const ULocale& locale, const std::u16string &separator)
{
    ::std::u16string sb;
    ::std::u16string conversionBuffer;
    if (!locale.getLanguage().empty()) {
        sb += *StringViewUtils::convert(&conversionBuffer, locale.getLanguage());
    }
    if (!locale.getScript().empty()) {
        sb += separator + *StringViewUtils::convert(&conversionBuffer, locale.getScript());
    }
    if (!locale.getCountry().empty()) {
        sb += separator + *StringViewUtils::convert(&conversionBuffer, locale.getCountry());
    }
    if (!locale.getVariant().empty()) {
        sb += separator + *StringViewUtils::convert(&conversionBuffer, locale.getVariant());
    }
    return sb;
}

::std::u16string LocaleUtils::toString(const ULocale& locale)
{
    return toString(locale, u"_");
}

::std::set<ULocale, std::less<>> LocaleUtils::getSupportedLocaleList()
{
    ::std::set<ULocale, std::less<>> supportedLocales;
    for (const auto& keyGroup : getSupportedLocaleMap()) {
        for (const auto& locale : keyGroup.second) {
            supportedLocales.insert(locale);
        }
    }
    return supportedLocales;
}

::std::map<::std::string, ::std::vector<ULocale>, std::less<>> LocaleUtils::getSupportedLocaleMap()
{
    return BASE_SUPPORTED_LOCALES();
}

ULocale LocaleUtils::createValidRegionAwareLanguage(const ULocale& language, const ULocale& region)
{
    if (language == region) {
        return language;
    }
    ULocale languageHybridLocale(language.getLanguage(), language.getScript(), region.getCountry());
    std::string_view languageHybridLocaleStr(languageHybridLocale.getName());
    int32_t start = 0;
    int32_t limit = unum_countAvailable() - 1;
    while (start <= limit) {
        int32_t mid = std::midpoint(start, limit);
        int32_t comp = languageHybridLocaleStr.compare(unum_getAvailable(mid));
        if (comp < 0) {
            limit = mid - 1;
        }
        else if (comp > 0) {
            start = mid + 1;
        }
        else {
            return languageHybridLocale;
        }
    }
    return language;
}

static ::std::map<ULocale, ::std::pair<ULocale, ULocale>>* initFALLBACK_PAIR_LOCALES()
{
    auto result = new ::std::map<ULocale, ::std::pair<ULocale, ULocale>>();
    auto fallbackMappings = inflection::resources::DataResource::getProperties(u"/org/unicode/inflection/locale/fallback-locales.properties");
    for (auto const &entry : fallbackMappings) {
        auto locales = LocaleUtils::toLocales(entry.second);
        npc(result)->emplace(StringViewUtils::to_string(entry.first), ::std::pair<ULocale, ULocale>(locales[0], locales[1]));
    }
    return result;
}

static const ::std::map<ULocale, ::std::pair<ULocale, ULocale>>* FALLBACK_PAIR_LOCALES()
{
    static auto FALLBACK_PAIR_LOCALES_ = initFALLBACK_PAIR_LOCALES();
    return FALLBACK_PAIR_LOCALES_;
}

::std::pair<ULocale, ULocale> LocaleUtils::getFallbackPair(const ULocale& locale)
{
    auto result = npc(FALLBACK_PAIR_LOCALES())->find(locale);
    if (result != npc(FALLBACK_PAIR_LOCALES())->end()) {
        return result->second;
    }
    return ::std::pair<ULocale, ULocale>(locale, locale.getFallback());
}

/* I have yet to see a locale go beyond 4 scripts. This should be enough for now. */
static constexpr int8_t MAX_SCRIPTS_TO_COMPARE = 8;

bool LocaleUtils::areLanguageScriptCompatible(const ULocale& locale1, const ULocale& locale2)
{
    auto match = (locale1 == locale2);
    if(!match) {
        auto localeFallback1 = locale1;
        auto localeFallback2 = locale2;
        auto result1 = npc(FALLBACK_PAIR_LOCALES())->find(locale1);
        if (result1 != npc(FALLBACK_PAIR_LOCALES())->end()) {
            localeFallback1 = result1->second.second;
        }
        auto result2 = npc(FALLBACK_PAIR_LOCALES())->find(locale2);
        if (result2 != npc(FALLBACK_PAIR_LOCALES())->end()) {
            localeFallback2 = result2->second.second;
        }
        match = (localeFallback1.getLanguage() == localeFallback2.getLanguage());
        if (match) {
            auto status = U_ZERO_ERROR;
            UScriptCode scripts1[MAX_SCRIPTS_TO_COMPARE] = {
                    USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE
            };
            UScriptCode scripts2[MAX_SCRIPTS_TO_COMPARE] = {
                    USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE, USCRIPT_INVALID_CODE
            };
            auto scripts1Len = uscript_getCode(locale1.getName().c_str(), scripts1, MAX_SCRIPTS_TO_COMPARE, &status);
            auto scripts2Len = uscript_getCode(locale2.getName().c_str(), scripts2, MAX_SCRIPTS_TO_COMPARE, &status);
            ::inflection::exception::ICUException::throwOnFailure(status);
            match = scripts1Len == scripts2Len && memcmp(scripts1, scripts2, scripts1Len * sizeof(UScriptCode)) == 0;
        }
    }
    return match;
}

} // namespace inflection::util
