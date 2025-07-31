/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LocaleUtils.hpp>

#include <inflection/util/ULocale.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/unum.h>
#include <unicode/uscript.h>
#include <algorithm>
#include <cstring>
#include <numeric>

namespace inflection::util {

const ULocale& LocaleUtils::ARABIC()
{
    static auto ARABIC_ = new ULocale("ar");
    return *npc(ARABIC_);
}

const ULocale& LocaleUtils::SAUDI_ARABIA()
{
    static auto SAUDI_ARABIA_ = new ULocale("ar", "SA");
    return *npc(SAUDI_ARABIA_);
}

const ::inflection::util::ULocale& LocaleUtils::BULGARIAN()
{
    static auto BULGARIAN_ = new ULocale("bg");
    return *npc(BULGARIAN_);
}

const ::inflection::util::ULocale& LocaleUtils::BULGARIA()
{
    static auto BULGARIA_ = new ULocale("bg", "BG");
    return *npc(BULGARIA_);
}

const ULocale& LocaleUtils::CATALAN()
{
    static auto CATALAN_ = new ULocale("ca");
    return *npc(CATALAN_);
}

const ULocale& LocaleUtils::SPAIN_CATALAN()
{
    static auto SPAIN_CATALAN_ = new ULocale("ca", "ES");
    return *npc(SPAIN_CATALAN_);
}

const ULocale& LocaleUtils::CZECH()
{
    static auto CZECH_ = new ULocale("cs");
    return *npc(CZECH_);
}

const ULocale& LocaleUtils::CZECHIA()
{
    static auto CZECH_REPUBLIC_ = new ULocale("cs", "CZ");
    return *npc(CZECH_REPUBLIC_);
}

const ULocale& LocaleUtils::DANISH()
{
    static auto DANISH_ = new ULocale("da");
    return *npc(DANISH_);
}

const ULocale& LocaleUtils::DENMARK()
{
    static auto DENMARK_ = new ULocale("da", "DK");
    return *npc(DENMARK_);
}

const ULocale& LocaleUtils::GERMAN()
{
    static auto GERMAN_ = new ULocale("de");
    return *npc(GERMAN_);
}

const ULocale& LocaleUtils::GERMANY()
{
    static auto GERMANY_ = new ULocale("de", "DE");
    return *npc(GERMANY_);
}

const ULocale& LocaleUtils::SWITZERLAND_GERMAN()
{
    static auto SWITZERLAND_GERMAN_ = new ULocale("de", "CH");
    return *npc(SWITZERLAND_GERMAN_);
}

const ULocale& LocaleUtils::AUSTRIA()
{
    static auto AUSTRIA_ = new ULocale("de", "AT");
    return *npc(AUSTRIA_);
}

const ULocale& LocaleUtils::GREEK()
{
    static auto GREEK_ = new ULocale("el");
    return *npc(GREEK_);
}

const ULocale& LocaleUtils::GREECE()
{
    static auto GREECE_ = new ULocale("el", "GR");
    return *npc(GREECE_);
}

const ULocale& LocaleUtils::ENGLISH()
{
    static auto ENGLISH_ = new ULocale("en");
    return *npc(ENGLISH_);
}

const ULocale& LocaleUtils::US()
{
    static auto US_ = new ULocale("en", "US");
    return *npc(US_);
}

const ULocale& LocaleUtils::AUSTRALIA_ENGLISH()
{
    static auto AUSTRALIA_ENGLISH_ = new ULocale("en", "AU");
    return *npc(AUSTRALIA_ENGLISH_);
}

const ULocale& LocaleUtils::CANADIAN_ENGLISH()
{
    static auto CANADIAN_ENGLISH_ = new ULocale("en", "CA");
    return *npc(CANADIAN_ENGLISH_);
}

const ULocale& LocaleUtils::SINGAPOREAN_ENGLISH()
{
    static auto SINGAPOREAN_ENGLISH_ = new ULocale("en", "SG");
    return *npc(SINGAPOREAN_ENGLISH_);
}

const ULocale& LocaleUtils::GREAT_BRITAIN()
{
    static auto GREAT_BRITAIN_ = new ULocale("en", "GB");
    return *npc(GREAT_BRITAIN_);
}

const ULocale& LocaleUtils::NEW_ZEALAND_ENGLISH()
{
    static auto NEW_ZEALAND_ENGLISH_ = new ULocale("en", "NZ");
    return *npc(NEW_ZEALAND_ENGLISH_);
}

const ULocale& LocaleUtils::INDIAN_ENGLISH()
{
    static auto INDIAN_ENGLISH_ = new ULocale("en", "IN");
    return *npc(INDIAN_ENGLISH_);
}

const ULocale& LocaleUtils::SOUTH_AFRICAN_ENGLISH()
{
    static auto SOUTH_AFRICAN_ENGLISH_ = new ULocale("en", "ZA");
    return *npc(SOUTH_AFRICAN_ENGLISH_);
}

const ULocale& LocaleUtils::IRISH_ENGLISH()
{
    static auto IRISH_ENGLISH_ = new ULocale("en", "IE");
    return *npc(IRISH_ENGLISH_);
}

const ULocale& LocaleUtils::SPANISH()
{
    static auto SPANISH_ = new ULocale("es");
    return *npc(SPANISH_);
}

const ULocale& LocaleUtils::SPAIN_SPANISH()
{
    static auto SPAIN_SPANISH_ = new ULocale("es", "ES");
    return *npc(SPAIN_SPANISH_);
}

const ULocale& LocaleUtils::MEXICO_SPANISH()
{
    static auto MEXICO_SPANISH_ = new ULocale("es", "MX");
    return *npc(MEXICO_SPANISH_);
}

const ULocale& LocaleUtils::US_SPANISH()
{
    static auto US_SPANISH_ = new ULocale("es", "US");
    return *npc(US_SPANISH_);
}

const ULocale& LocaleUtils::CHILEAN_SPANISH()
{
    static auto CHILEAN_SPANISH_ = new ULocale("es", "CL");
    return *npc(CHILEAN_SPANISH_);
}

const ULocale& LocaleUtils::ESTONIAN()
{
    static auto ESTONIAN_ = new ULocale("et");
    return *npc(ESTONIAN_);
}

const ULocale& LocaleUtils::ESTONIA()
{
    static auto ESTONIA_ = new ULocale("et", "EE");
    return *npc(ESTONIA_);
}

const ULocale& LocaleUtils::FINNISH()
{
    static auto FINNISH_ = new ULocale("fi");
    return *npc(FINNISH_);
}

const ULocale& LocaleUtils::FINLAND()
{
    static auto FINLAND_ = new ULocale("fi", "FI");
    return *npc(FINLAND_);
}

const ULocale& LocaleUtils::FRENCH()
{
    static auto FRENCH_ = new ULocale("fr");
    return *npc(FRENCH_);
}

const ULocale& LocaleUtils::FRANCE()
{
    static auto FRANCE_ = new ULocale("fr", "FR");
    return *npc(FRANCE_);
}

const ULocale& LocaleUtils::BELGIUM_FRENCH()
{
    static auto BELGIUM_FRENCH_ = new ULocale("fr", "BE");
    return *npc(BELGIUM_FRENCH_);
}

const ULocale& LocaleUtils::SWITZERLAND_FRENCH()
{
    static auto SWITZERLAND_FRENCH_ = new ULocale("fr", "CH");
    return *npc(SWITZERLAND_FRENCH_);
}

const ULocale& LocaleUtils::CANADIAN_FRENCH()
{
    static auto CANADIAN_FRENCH_ = new ULocale("fr", "CA");
    return *npc(CANADIAN_FRENCH_);
}

const ULocale& LocaleUtils::HEBREW()
{
    static auto HEBREW_ = new ULocale("he");
    return *npc(HEBREW_);
}

const ULocale& LocaleUtils::ISRAEL()
{
    static auto ISRAEL_ = new ULocale("he", "IL");
    return *npc(ISRAEL_);
}

const ULocale& LocaleUtils::HINDI()
{
    static auto HINDI_ = new ULocale("hi");
    return *npc(HINDI_);
}

const ULocale& LocaleUtils::INDIA_HINDI()
{
    static auto INDIA_HINDI_ = new ULocale("hi", "IN");
    return *npc(INDIA_HINDI_);
}

const ULocale& LocaleUtils::CROATIAN()
{
    static auto CROATIAN_ = new ULocale("hr");
    return *npc(CROATIAN_);
}

const ULocale& LocaleUtils::CROATIA()
{
    static auto CROATIA_ = new ULocale("hr", "HR");
    return *npc(CROATIA_);
}

const ULocale& LocaleUtils::HUNGARIAN()
{
    static auto HUNGARIAN_ = new ULocale("hu");
    return *npc(HUNGARIAN_);
}

const ULocale& LocaleUtils::HUNGARY()
{
    static auto HUNGARY_ = new ULocale("hu", "HU");
    return *npc(HUNGARY_);
}

const ULocale& LocaleUtils::INDONESIAN()
{
    static auto INDONESIAN_ = new ULocale("id");
    return *npc(INDONESIAN_);
}

const ULocale& LocaleUtils::INDONESIA()
{
    static auto INDONESIA_ = new ULocale("id", "ID");
    return *npc(INDONESIA_);
}

const ULocale& LocaleUtils::ICELANDIC()
{
    static auto ICELANDIC_ = new ULocale("is");
    return *npc(ICELANDIC_);
}

const ULocale& LocaleUtils::ICELAND()
{
    static auto ICELAND_ = new ULocale("is", "IS");
    return *npc(ICELAND_);
}

const ULocale& LocaleUtils::ITALIAN()
{
    static auto ITALIAN_ = new ULocale("it");
    return *npc(ITALIAN_);
}

const ULocale& LocaleUtils::ITALY()
{
    static auto ITALY_ = new ULocale("it", "IT");
    return *npc(ITALY_);
}

const ULocale& LocaleUtils::SWITZERLAND_ITALIAN()
{
    static auto SWITZERLAND_ITALIAN_ = new ULocale("it", "CH");
    return *npc(SWITZERLAND_ITALIAN_);
}

const ULocale& LocaleUtils::JAPANESE()
{
    static auto JAPANESE_ = new ULocale("ja");
    return *npc(JAPANESE_);
}

const ULocale& LocaleUtils::JAPAN()
{
    static auto JAPAN_ = new ULocale("ja", "JP");
    return *npc(JAPAN_);
}

const ULocale& LocaleUtils::KAZAKH()
{
    static auto KAZAKH_ = new ULocale("kk");
    return *npc(KAZAKH_);
}

const ULocale& LocaleUtils::KAZAKHSTAN()
{
    static auto KAZAKHSTAN_ = new ULocale("kk", "KZ");
    return *npc(KAZAKHSTAN_);
}

const ULocale& LocaleUtils::KOREAN()
{
    static auto KOREAN_ = new ULocale("ko");
    return *npc(KOREAN_);
}

const ULocale& LocaleUtils::KOREA()
{
    static auto KOREA_ = new ULocale("ko", "KR");
    return *npc(KOREA_);
}

const ULocale& LocaleUtils::LITHUANIAN()
{
    static auto LITHUANIAN_ = new ULocale("lt");
    return *npc(LITHUANIAN_);
}

const ULocale& LocaleUtils::LITHUANIA()
{
    static auto LITHUANIA_ = new ULocale("lt", "LT");
    return *npc(LITHUANIA_);
}

const ULocale& LocaleUtils::MALAY()
{
    static auto MALAY_ = new ULocale("ms");
    return *npc(MALAY_);
}

const ULocale& LocaleUtils::MALAYSIA()
{
    static auto MALAYSIA_ = new ULocale("ms", "MY");
    return *npc(MALAYSIA_);
}

const ULocale& LocaleUtils::NORWEGIAN()
{
    static auto NORWEGIAN_ = new ULocale("nb");
    return *npc(NORWEGIAN_);
}

const ULocale& LocaleUtils::NORWAY()
{
    static auto NORWAY_ = new ULocale("nb", "NO");
    return *npc(NORWAY_);
}

const ULocale& LocaleUtils::DUTCH()
{
    static auto DUTCH_ = new ULocale("nl");
    return *npc(DUTCH_);
}

const ULocale& LocaleUtils::NETHERLANDS()
{
    static auto NETHERLANDS_ = new ULocale("nl", "NL");
    return *npc(NETHERLANDS_);
}

const ULocale& LocaleUtils::BELGIUM_DUTCH()
{
    static auto BELGIUM_DUTCH_ = new ULocale("nl", "BE");
    return *npc(BELGIUM_DUTCH_);
}

const ULocale& LocaleUtils::POLISH()
{
    static auto POLISH_ = new ULocale("pl");
    return *npc(POLISH_);
}

const ULocale& LocaleUtils::POLAND()
{
    static auto POLAND_ = new ULocale("pl", "PL");
    return *npc(POLAND_);
}

const ULocale& LocaleUtils::PORTUGUESE()
{
    static auto PORTUGUESE_ = new ULocale("pt");
    return *npc(PORTUGUESE_);
}

const ULocale& LocaleUtils::PORTUGAL()
{
    static auto PORTUGAL_ = new ULocale("pt", "PT");
    return *npc(PORTUGAL_);
}

const ULocale& LocaleUtils::BRAZIL()
{
    static auto BRAZIL_ = new ULocale("pt", "BR");
    return *npc(BRAZIL_);
}

const ULocale& LocaleUtils::ROMANIAN()
{
    static auto ROMANIAN_ = new ULocale("ro");
    return *npc(ROMANIAN_);
}

const ULocale& LocaleUtils::ROMANIA()
{
    static auto ROMANIA_ = new ULocale("ro", "RO");
    return *npc(ROMANIA_);
}

const ULocale& LocaleUtils::RUSSIAN()
{
    static auto RUSSIAN_ = new ULocale("ru");
    return *npc(RUSSIAN_);
}

const ULocale& LocaleUtils::RUSSIA()
{
    static auto RUSSIA_ = new ULocale("ru", "RU");
    return *npc(RUSSIA_);
}

const ULocale& LocaleUtils::SLOVAK()
{
    static auto SLOVAK_ = new ULocale("sk");
    return *npc(SLOVAK_);
}

const ULocale& LocaleUtils::SLOVAKIA()
{
    static auto SLOVAKIA_ = new ULocale("sk", "SK");
    return *npc(SLOVAKIA_);
}

const ULocale& LocaleUtils::SERBIAN()
{
    static auto SERBIAN_ = new ULocale("sr");
    return *npc(SERBIAN_);
}

const ULocale& LocaleUtils::SERBIA()
{
    static auto SERBIA_ = new ULocale("sr", "RS");
    return *npc(SERBIA_);
}

const ULocale& LocaleUtils::SWEDISH()
{
    static auto SWEDISH_ = new ULocale("sv");
    return *npc(SWEDISH_);
}

const ULocale& LocaleUtils::SWEDEN()
{
    static auto SWEDEN_ = new ULocale("sv", "SE");
    return *npc(SWEDEN_);
}

const ULocale& LocaleUtils::THAI()
{
    static auto THAI_ = new ULocale("th");
    return *npc(THAI_);
}

const ULocale& LocaleUtils::THAILAND()
{
    static auto THAILAND_ = new ULocale("th", "TH");
    return *npc(THAILAND_);
}

const ULocale& LocaleUtils::TURKISH()
{
    static auto TURKISH_ = new ULocale("tr");
    return *npc(TURKISH_);
}

const ULocale& LocaleUtils::TURKEY()
{
    static auto TURKEY_ = new ULocale("tr", "TR");
    return *npc(TURKEY_);
}

const ULocale& LocaleUtils::UKRAINIAN()
{
    static auto UKRAINIAN_ = new ULocale("uk");
    return *npc(UKRAINIAN_);
}

const ULocale& LocaleUtils::UKRAINE()
{
    static auto UKRAINE_ = new ULocale("uk", "UA");
    return *npc(UKRAINE_);
}

const ULocale& LocaleUtils::VIETNAMESE()
{
    static auto VIETNAMESE_ = new ULocale("vi");
    return *npc(VIETNAMESE_);
}

const ULocale& LocaleUtils::VIETNAM()
{
    static auto VIETNAM_ = new ULocale("vi", "VN");
    return *npc(VIETNAM_);
}

const ULocale& LocaleUtils::SHANGHAINESE()
{
    static auto SHANGHAINESE_ = new ULocale("wuu");
    return *npc(SHANGHAINESE_);
}

const ULocale& LocaleUtils::CHINA_SHANGHAINESE()
{
    static auto CHINA_SHANGHAINESE_ = new ULocale("wuu", "CN");
    return *npc(CHINA_SHANGHAINESE_);
}

const ULocale& LocaleUtils::CANTONESE()
{
    static auto CANTONESE_ = new ULocale("yue");
    return *npc(CANTONESE_);
}

const ULocale& LocaleUtils::CHINA_CANTONESE()
{
    static auto CHINA_CANTONESE_ = new ULocale("yue", "CN");
    return *npc(CHINA_CANTONESE_);
}

const ULocale& LocaleUtils::HONGKONG_CANTONESE()
{
    static auto HONGKONG_CANTONESE_ = new ULocale("yue", "HK");
    return *npc(HONGKONG_CANTONESE_);
}

const ULocale& LocaleUtils::CHINESE()
{
    static auto CHINESE_ = new ULocale("zh");
    return *npc(CHINESE_);
}

const ULocale& LocaleUtils::SIMPLIFIED_CHINESE()
{
    static auto SIMPLIFIED_CHINESE_ = new ULocale("zh", "Hans");
    return *npc(SIMPLIFIED_CHINESE_);
}

const ULocale& LocaleUtils::TRADITIONAL_CHINESE()
{
    static auto TRADITIONAL_CHINESE_ = new ULocale("zh", "Hant");
    return *npc(TRADITIONAL_CHINESE_);
}

const ULocale& LocaleUtils::HONGKONG_CHINESE()
{
    static auto HONGKONG_CHINESE_ = new ULocale("zh", "HK");
    return *npc(HONGKONG_CHINESE_);
}

const ULocale& LocaleUtils::CHINA()
{
    static auto CHINA_ = new ULocale("zh", "CN");
    return *npc(CHINA_);
}

const ULocale& LocaleUtils::TAIWAN()
{
    static auto TAIWAN_ = new ULocale("zh", "TW");
    return *npc(TAIWAN_);
}

const ULocale& LocaleUtils::ROOT()
{
    static auto ROOT_ = new ULocale("");
    return *npc(ROOT_);
}

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
