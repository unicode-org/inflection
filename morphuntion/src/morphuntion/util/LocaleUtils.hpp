/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Provides utility functions for use with ULocale objects.
 * @details This class contains many static functions that are very useful for manipulation, combination, accessing, and
 * validation of ULocale objects.
 *
 * This class also has functions for getting an instance of ULocale for **all of Morphuntion's supported locales**, as well
 * as a list of all currently supported locales (getSupportedLocaleList()).
 */
class MORPHUNTION_CLASS_API morphuntion::util::LocaleUtils final
{
public:
    /**
     * Converts a <code>std::u16string</code> representing a locale into a ULocale object. This function requires a the
     * input string to be either in the ICU style C locale (e.g. "en_US") or the two-letter or three-letter ISO-639 code
     * (see ULocale::ULocale()).
     *
     * @param localeString The input string to construct a ULocale from.
     * @return The ULocale representing the input string.
     */
    static ::morphuntion::util::ULocale toLocale(const ::std::u16string& localeString);
    /**
     * Converts an array of <code>std::u16string</code> representing locales into a <code>std::vector</code> of ULocale
     * objects. This function requires a the input strings to be either in the ICU style C locale (e.g. "en_US") or the
     * two-letter or three-letter ISO-639 code (see ULocale::ULocale()).
     *
     * @param localesString The input strings to construct an array of @ref ULocale "ULocale"s from.
     * @return A <code>std::vector</code> of @ref ULocale "ULocale"s representing the input string.
     */
    static ::std::vector<::morphuntion::util::ULocale> toLocales(const ::std::u16string & localesString);
    /**
     * Converts a ULocale into a <code>std::u16string</code> representation with a given separator. The string is
     * formatted as such:<br/>
     * <code>%1,%2,%3,%4</code><br/>
     * Where:<br/>
     * %1 = The language<br/>
     * %2 = The script<br/>
     * %3 = The country<br/>
     * %4 = The variant<br/>
     * , = The dileanation.
     *
     * @param locale The ULocale object to represent as a <code>std::u16string</code>.
     * @param separator The dileantion characters to use.
     * @return The ULocale object represented as a <code>std::u16string</code>.
     */
    static ::std::u16string toString(const ::morphuntion::util::ULocale& locale, const ::std::u16string& separator);
    /**
     * Converts a ULocale into a <code>std::u16string</code> representation with the separator "_". The string is
     * formatted as such:<br/>
     * <code>%1_%2_%3_%4</code><br/>
     * Where:<br/>
     * %1 = The language<br/>
     * %2 = The script<br/>
     * %3 = The country<br/>
     * %4 = The variant.
     *
     * @param locale The ULocale object to represent as a <code>std::u16string</code>.
     * @return The ULocale object represented as a <code>std::u16string</code>.
     */
    static ::std::u16string toString(const ::morphuntion::util::ULocale& locale);
    /**
     * Returns the list of supported locales in Morphuntion as a <code>std::set<ULocale></code>.
     * @return The list of supported locales in Morphuntion.
     */
    static ::std::set<::morphuntion::util::ULocale> getSupportedLocaleList();
    /**
     * Returns a map of the supported locales in Morphuntion, mapping the language string in the configuration file to the
     * ULocale object it represents.
     *
     * @return The map of supported locales in Morphuntion.
     */
    static ::std::map<::std::string, ::std::vector<::morphuntion::util::ULocale>, std::greater<>> getSupportedLocaleMap();

    /**
     * This allows you to have behavior that is close to the spoken language, without forcing an unsupported language variant from being too strictly the region based language.
     * If your iOS language is en_US, but your spoken language is ar_SA, just return ar_SA because ar_US is not valid in CLDR.
     * If your iOS language is ar_TN, but your spoken language is ar_SA, return ar_TN because it's valid in CLDR and ICU.
     * For example, ar_TN uses 0-9 for the digits, but ar_SA uses ٠-٩ for the digits.
     * If your iOS language is es_MX, but your spoken language is es_CO, return es_MX because it's valid in CLDR and ICU.
     * For example, es_MX uses "." for the decimal point, but es_CO uses "," for the decimal point.
     * @param language The spoken language setting
     * @param region The region setting from the device, which includes the iOS language
     * @return A hybrid locale
     */
    static ::morphuntion::util::ULocale createValidRegionAwareLanguage(const ::morphuntion::util::ULocale& language, const ::morphuntion::util::ULocale& region);
    /**
     * Attempts to fallback to a region specific locale when the region-specific locale is shifted to a region-less
     * locale. This is currently used to get fallbacks for Chinese dialects.
     *
     * @param locale The region-less locale to find a fallback for.
     * @return The fallback locale pair.
     */
    static ::std::pair<::morphuntion::util::ULocale, ::morphuntion::util::ULocale> getFallbackPair(const ::morphuntion::util::ULocale& locale);
    /**
     * Returns True if the two locales have a compatible script (written form) and language (spoken form).
     *
     * @param locale1 The first locale to compare.
     * @param locale2 The second locale to compare.
     * @return True if the two locales have a compatible script and language.
     */
    static bool areLanguageScriptCompatible(const ::morphuntion::util::ULocale& locale1, const ::morphuntion::util::ULocale& locale2);

private:
    LocaleUtils() = delete;

public:
    /**
     * ar: Arabic
     */
    static const ::morphuntion::util::ULocale& ARABIC();
    /**
     * ar_SA: Arabic (Saudi Arabia)
     */
    static const ::morphuntion::util::ULocale& SAUDI_ARABIA();
    /**
     * bg: Bulgarian
     */
    static const ::morphuntion::util::ULocale& BULGARIAN();
    /**
     * bg_BG: Bulgarian (Bulgaria)
     */
    static const ::morphuntion::util::ULocale& BULGARIA();
    /**
     * ca: Catalan
     */
    static const ::morphuntion::util::ULocale& CATALAN();
    /**
     * ca_ES: Catalan (Spain)
     */
    static const ::morphuntion::util::ULocale& SPAIN_CATALAN();
    /**
     * cs: Czech
     */
    static const ::morphuntion::util::ULocale& CZECH();
    /**
     * cs: Czech (Czechia or Czech Republic)
     */
    static const ::morphuntion::util::ULocale& CZECHIA();
    /**
     * da: Danish
     */
    static const ::morphuntion::util::ULocale& DANISH();
    /**
     * da_DK: Danish (Denmark)
     */
    static const ::morphuntion::util::ULocale& DENMARK();
    /**
     * de: German
     */
    static const ::morphuntion::util::ULocale& GERMAN();
    /**
     * de_DE: German (Germany)
     */
    static const ::morphuntion::util::ULocale& GERMANY();
    /**
     * de_CH: German (Switzerland)
     */
    static const ::morphuntion::util::ULocale& SWITZERLAND_GERMAN();
    /**
     * de_AT: German (Austria)
     */
    static const ::morphuntion::util::ULocale& AUSTRIA();
    /**
     * el: Greek
     */
    static const ::morphuntion::util::ULocale& GREEK();
    /**
     * el_GR: Greek (Greece)
     */
    static const ::morphuntion::util::ULocale& GREECE();
    /**
     * en: English
     */
    static const ::morphuntion::util::ULocale& ENGLISH();
    /**
     * en_US: English (United States)
     */
    static const ::morphuntion::util::ULocale& US();
    /**
     * en_AU: English (Australia)
     */
    static const ::morphuntion::util::ULocale& AUSTRALIA_ENGLISH();
    /**
     * en_CA: English (Canada)
     */
    static const ::morphuntion::util::ULocale& CANADIAN_ENGLISH();
    /**
     * en_SG: English (Singapore)
     */
    static const ::morphuntion::util::ULocale& SINGAPOREAN_ENGLISH();
    /**
     * en_GB: English (United Kingdom)
     * It's worth noting that the United Kingdom includes Great Britain and Northern Ireland.
     */
    static const ::morphuntion::util::ULocale& GREAT_BRITAIN();
    /**
     * en_NZ: English (New Zealand)
     */
    static const ::morphuntion::util::ULocale& NEW_ZEALAND_ENGLISH();
    /**
     * en_IN: English (India)
     */
    static const ::morphuntion::util::ULocale& INDIAN_ENGLISH();
    /**
     * en_ZA: English (South Africa)
     */
    static const ::morphuntion::util::ULocale& SOUTH_AFRICAN_ENGLISH();
    /**
     * en_IE: English (Ireland)
     */
    static const ::morphuntion::util::ULocale& IRISH_ENGLISH();
    /**
     * es: Spanish
     */
    static const ::morphuntion::util::ULocale& SPANISH();
    /**
     * es_ES: Spanish (Spain)
     */
    static const ::morphuntion::util::ULocale& SPAIN_SPANISH();
    /**
     * es_MX: Spanish (Mexico)
     */
    static const ::morphuntion::util::ULocale& MEXICO_SPANISH();
    /**
     * es_US: Spanish (United States)
     */
    static const ::morphuntion::util::ULocale& US_SPANISH();
    /**
     * es_CL: Spanish (Chile)
     */
    static const ::morphuntion::util::ULocale& CHILEAN_SPANISH();
    /**
     * et: Estonian
     */
    static const ::morphuntion::util::ULocale& ESTONIAN();
    /**
     * et_EE: Estonian (Estonia)
     */
    static const ::morphuntion::util::ULocale& ESTONIA();
    /**
     * fi: Finnish
     */
    static const ::morphuntion::util::ULocale& FINNISH();
    /**
     * fi_FI: Finnish (Finland)
     */
    static const ::morphuntion::util::ULocale& FINLAND();
    /**
     * fr: French
     */
    static const ::morphuntion::util::ULocale& FRENCH();
    /**
     * fr_FR: French (France)
     */
    static const ::morphuntion::util::ULocale& FRANCE();
    /**
     * fr_BE: French (Belgium)
     */
    static const ::morphuntion::util::ULocale& BELGIUM_FRENCH();
    /**
     * fr_CH: French (Switzerland)
     */
    static const ::morphuntion::util::ULocale& SWITZERLAND_FRENCH();
    /**
     * fr_CA: French (Canada)
     */
    static const ::morphuntion::util::ULocale& CANADIAN_FRENCH();
    /**
     * he: Hebrew
     */
    static const ::morphuntion::util::ULocale& HEBREW();
    /**
     * he_IL: Hebrew (Israel)
     */
    static const ::morphuntion::util::ULocale& ISRAEL();
    /**
     * hi: Hindi
     */
    static const ::morphuntion::util::ULocale& HINDI();
    /**
     * hi_IN: Hindi (India)
     */
    static const ::morphuntion::util::ULocale& INDIA_HINDI();
    /**
     * hr: Croatian
     */
    static const ::morphuntion::util::ULocale& CROATIAN();
    /**
     * hr_HR: Croatian (Croatia)
     */
    static const ::morphuntion::util::ULocale& CROATIA();
    /**
     * hu: Hungarian
     */
    static const ::morphuntion::util::ULocale& HUNGARIAN();
    /**
     * hu_HU: Hungarian (Hungary)
     */
    static const ::morphuntion::util::ULocale& HUNGARY();
    /**
     * id: Indonesian
     */
    static const ::morphuntion::util::ULocale& INDONESIAN();
    /**
     * id_ID: Indonesian (Indonesia)
     */
    static const ::morphuntion::util::ULocale& INDONESIA();
    /**
     * is: Icelandic
     */
    static const ::morphuntion::util::ULocale& ICELANDIC();
    /**
     * is_IS: Icelandic (Iceland)
     */
    static const ::morphuntion::util::ULocale& ICELAND();
    /**
     * it: Italian
     */
    static const ::morphuntion::util::ULocale& ITALIAN();
    /**
     * it_IT: Italian (Italy)
     */
    static const ::morphuntion::util::ULocale& ITALY();
    /**
     * it_CH: Italian (Switzerland)
     */
    static const ::morphuntion::util::ULocale& SWITZERLAND_ITALIAN();
    /**
     * ja: Japanese
     */
    static const ::morphuntion::util::ULocale& JAPANESE();
    /**
     * ja_JP: Japanese (Japan)
     */
    static const ::morphuntion::util::ULocale& JAPAN();
    /**
     * kk: Kazakh
     */
    static const ::morphuntion::util::ULocale& KAZAKH();
    /**
     * kk_KZ: Kazakh (Kazakhstan)
     */
    static const ::morphuntion::util::ULocale& KAZAKHSTAN();
    /**
     * ko: Korean
     */
    static const ::morphuntion::util::ULocale& KOREAN();
    /**
     * ko_KR: Korean (South Korea)
     */
    static const ::morphuntion::util::ULocale& KOREA();
    /**
     * lt: Lithuanian
     */
    static const ::morphuntion::util::ULocale& LITHUANIAN();
    /**
     * lt_LT: Lithuanian (Lithuania)
     */
    static const ::morphuntion::util::ULocale& LITHUANIA();
    /**
     * ms: Malay
     */
    static const ::morphuntion::util::ULocale& MALAY();
    /**
     * ms_MY: Malay (Malaysia)
     */
    static const ::morphuntion::util::ULocale& MALAYSIA();
    /**
     * nb: Norwegian Bokmål
     */
    static const ::morphuntion::util::ULocale& NORWEGIAN();
    /**
     * nb_NO: Norwegian Bokmål (Norway)
     */
    static const ::morphuntion::util::ULocale& NORWAY();
    /**
     * nl: Dutch
     */
    static const ::morphuntion::util::ULocale& DUTCH();
    /**
     * nl_NL: Dutch (Netherlands)
     */
    static const ::morphuntion::util::ULocale& NETHERLANDS();
    /**
     * nl_BE: Dutch (Belgium)
     */
    static const ::morphuntion::util::ULocale& BELGIUM_DUTCH();
    /**
     * pl: Polish
     */
    static const ::morphuntion::util::ULocale& POLISH();
    /**
     * pl_PL: Polish (Poland)
     */
    static const ::morphuntion::util::ULocale& POLAND();
    /**
     * pt: Portuguese
     */
    static const ::morphuntion::util::ULocale& PORTUGUESE();
    /**
     * pt_BR: Portuguese (Portugal)
     */
    static const ::morphuntion::util::ULocale& PORTUGAL();
    /**
     * pt_BR: Portuguese (Brazil)
     */
    static const ::morphuntion::util::ULocale& BRAZIL();
    /**
     * ro: Romanian
     */
    static const ::morphuntion::util::ULocale& ROMANIAN();
    /**
     * ro_RO: Romanian (Romania)
     */
    static const ::morphuntion::util::ULocale& ROMANIA();
    /**
     * ru: Russian
     */
    static const ::morphuntion::util::ULocale& RUSSIAN();
    /**
     * ru_RU: Russian (Russia)
     */
    static const ::morphuntion::util::ULocale& RUSSIA();
    /**
     * sk: Slovak
     */
    static const ::morphuntion::util::ULocale& SLOVAK();
    /**
     * sk_SK: Slovak (Slovakia)
     */
    static const ::morphuntion::util::ULocale& SLOVAKIA();
    /**
     * sv: Swedish
     */
    static const ::morphuntion::util::ULocale& SWEDISH();
    /**
     * sv_SE: Swedish (Sweden)
     */
    static const ::morphuntion::util::ULocale& SWEDEN();
    /**
     * th: Thai
     */
    static const ::morphuntion::util::ULocale& THAI();
    /**
     * th_TH: Thai (Thailand)
     */
    static const ::morphuntion::util::ULocale& THAILAND();
    /**
     * tr: Turkish
     */
    static const ::morphuntion::util::ULocale& TURKISH();
    /**
     * tr_TR: Turkish (Turkey)
     */
    static const ::morphuntion::util::ULocale& TURKEY();
    /**
     * uk: Ukrainian
     */
    static const ::morphuntion::util::ULocale& UKRAINIAN();
    /**
     * uk_UA: Ukrainian (Ukraine)
     */
    static const ::morphuntion::util::ULocale& UKRAINE();
    static const ::morphuntion::util::ULocale& VIETNAMESE();
    static const ::morphuntion::util::ULocale& VIETNAM();
    static const ::morphuntion::util::ULocale& SHANGHAINESE();
    static const ::morphuntion::util::ULocale& CHINA_SHANGHAINESE();
    static const ::morphuntion::util::ULocale& CANTONESE();
    /**
     * yue_CN: Cantonese (China)
     * This represents the Cantonese written in the simplified Han script in China.
     */
    static const ::morphuntion::util::ULocale& CHINA_CANTONESE();
    /**
     * yue_HK: Cantonese (Hong Kong)
     * This represents the Cantonese written in the traditional Han script in Hong Kong.
     */
    static const ::morphuntion::util::ULocale& HONGKONG_CANTONESE();
    /**
     * zh: Chinese
     * This represents the Chinese macro-language. By default, it represents Simplified Mandarin in China.
     */
    static const ::morphuntion::util::ULocale& CHINESE();
    /**
     * zh_Hans: Chinese written in the simplified Han script.
     * This represents the Chinese macro-language written in the simplified Han script. By default, it represents Simplified Mandarin.
     */
    static const ::morphuntion::util::ULocale& SIMPLIFIED_CHINESE();
    /**
     * zh_Hant: Chinese written in the traditional Han script
     * This represents the Chinese macro-language written in the traditional Han script. By default, it represents Traditional Mandarin.
     */
    static const ::morphuntion::util::ULocale& TRADITIONAL_CHINESE();
    /**
     * zh_HK: Chinese (Hong Kong)
     * This represents the Chinese macro-language written in the traditional Han script in Hong Kong. By default, it represents Traditional Cantonese.
     */
    static const ::morphuntion::util::ULocale& HONGKONG_CHINESE();
    /**
     * zh_CN: Chinese (China)
     * This represents the Chinese macro-language in China. By default, it represents Simplified Mandarin in China.
     */
    static const ::morphuntion::util::ULocale& CHINA();
    /**
     * zh_TW: Chinese (Taiwan)
     * This represents the Chinese macro-language in Taiwan. By default, it represents Traditional Mandarin in Taiwan.
     */
    static const ::morphuntion::util::ULocale& TAIWAN();
    /**
     * The root locale is the locale that all other locales derive their data.
     * It does not represent any particular language or region.
     */
    static const ::morphuntion::util::ULocale& ROOT();

private:
    static const ::std::map<::std::string, ::std::vector<::morphuntion::util::ULocale>, std::greater<>>& BASE_SUPPORTED_LOCALES();
};
