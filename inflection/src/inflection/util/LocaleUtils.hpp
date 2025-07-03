/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
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
 * This class also has functions for getting an instance of ULocale for **all of Inflection's supported locales**, as well
 * as a list of all currently supported locales (getSupportedLocaleList()).
 */
class INFLECTION_CLASS_API inflection::util::LocaleUtils final
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
    static ::inflection::util::ULocale toLocale(const ::std::u16string& localeString);
    /**
     * Converts an array of <code>std::u16string</code> representing locales into a <code>std::vector</code> of ULocale
     * objects. This function requires a the input strings to be either in the ICU style C locale (e.g. "en_US") or the
     * two-letter or three-letter ISO-639 code (see ULocale::ULocale()).
     *
     * @param localesString The input strings to construct an array of @ref ULocale "ULocale"s from.
     * @return A <code>std::vector</code> of @ref ULocale "ULocale"s representing the input string.
     */
    static ::std::vector<::inflection::util::ULocale> toLocales(const ::std::u16string & localesString);
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
    static ::std::u16string toString(const ::inflection::util::ULocale& locale, const ::std::u16string& separator);
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
    static ::std::u16string toString(const ::inflection::util::ULocale& locale);
    /**
     * Returns the list of supported locales in Inflection as a <code>std::set<ULocale></code>.
     * @return The list of supported locales in Inflection.
     */
    static ::std::set<::inflection::util::ULocale, std::greater<>> getSupportedLocaleList();
    /**
     * Returns a map of the supported locales in Inflection, mapping the language string in the configuration file to the
     * ULocale object it represents.
     *
     * @return The map of supported locales in Inflection.
     */
    static ::std::map<::std::string, ::std::vector<::inflection::util::ULocale>, std::greater<>> getSupportedLocaleMap();

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
    static ::inflection::util::ULocale createValidRegionAwareLanguage(const ::inflection::util::ULocale& language, const ::inflection::util::ULocale& region);
    /**
     * Attempts to fallback to a region specific locale when the region-specific locale is shifted to a region-less
     * locale. This is currently used to get fallbacks for Chinese dialects.
     *
     * @param locale The region-less locale to find a fallback for.
     * @return The fallback locale pair.
     */
    static ::std::pair<::inflection::util::ULocale, ::inflection::util::ULocale> getFallbackPair(const ::inflection::util::ULocale& locale);
    /**
     * Returns True if the two locales have a compatible script (written form) and language (spoken form).
     *
     * @param locale1 The first locale to compare.
     * @param locale2 The second locale to compare.
     * @return True if the two locales have a compatible script and language.
     */
    static bool areLanguageScriptCompatible(const ::inflection::util::ULocale& locale1, const ::inflection::util::ULocale& locale2);

private:
    LocaleUtils() = delete;

public:
    /**
     * ar: Arabic
     */
    static const ::inflection::util::ULocale& ARABIC();
    /**
     * ar_SA: Arabic (Saudi Arabia)
     */
    static const ::inflection::util::ULocale& SAUDI_ARABIA();
    /**
     * bg: Bulgarian
     */
    static const ::inflection::util::ULocale& BULGARIAN();
    /**
     * bg_BG: Bulgarian (Bulgaria)
     */
    static const ::inflection::util::ULocale& BULGARIA();
    /**
     * ca: Catalan
     */
    static const ::inflection::util::ULocale& CATALAN();
    /**
     * ca_ES: Catalan (Spain)
     */
    static const ::inflection::util::ULocale& SPAIN_CATALAN();
    /**
     * cs: Czech
     */
    static const ::inflection::util::ULocale& CZECH();
    /**
     * cs: Czech (Czechia or Czech Republic)
     */
    static const ::inflection::util::ULocale& CZECHIA();
    /**
     * da: Danish
     */
    static const ::inflection::util::ULocale& DANISH();
    /**
     * da_DK: Danish (Denmark)
     */
    static const ::inflection::util::ULocale& DENMARK();
    /**
     * de: German
     */
    static const ::inflection::util::ULocale& GERMAN();
    /**
     * de_DE: German (Germany)
     */
    static const ::inflection::util::ULocale& GERMANY();
    /**
     * de_CH: German (Switzerland)
     */
    static const ::inflection::util::ULocale& SWITZERLAND_GERMAN();
    /**
     * de_AT: German (Austria)
     */
    static const ::inflection::util::ULocale& AUSTRIA();
    /**
     * el: Greek
     */
    static const ::inflection::util::ULocale& GREEK();
    /**
     * el_GR: Greek (Greece)
     */
    static const ::inflection::util::ULocale& GREECE();
    /**
     * en: English
     */
    static const ::inflection::util::ULocale& ENGLISH();
    /**
     * en_US: English (United States)
     */
    static const ::inflection::util::ULocale& US();
    /**
     * en_AU: English (Australia)
     */
    static const ::inflection::util::ULocale& AUSTRALIA_ENGLISH();
    /**
     * en_CA: English (Canada)
     */
    static const ::inflection::util::ULocale& CANADIAN_ENGLISH();
    /**
     * en_SG: English (Singapore)
     */
    static const ::inflection::util::ULocale& SINGAPOREAN_ENGLISH();
    /**
     * en_GB: English (United Kingdom)
     * It's worth noting that the United Kingdom includes Great Britain and Northern Ireland.
     */
    static const ::inflection::util::ULocale& GREAT_BRITAIN();
    /**
     * en_NZ: English (New Zealand)
     */
    static const ::inflection::util::ULocale& NEW_ZEALAND_ENGLISH();
    /**
     * en_IN: English (India)
     */
    static const ::inflection::util::ULocale& INDIAN_ENGLISH();
    /**
     * en_ZA: English (South Africa)
     */
    static const ::inflection::util::ULocale& SOUTH_AFRICAN_ENGLISH();
    /**
     * en_IE: English (Ireland)
     */
    static const ::inflection::util::ULocale& IRISH_ENGLISH();
    /**
     * es: Spanish
     */
    static const ::inflection::util::ULocale& SPANISH();
    /**
     * es_ES: Spanish (Spain)
     */
    static const ::inflection::util::ULocale& SPAIN_SPANISH();
    /**
     * es_MX: Spanish (Mexico)
     */
    static const ::inflection::util::ULocale& MEXICO_SPANISH();
    /**
     * es_US: Spanish (United States)
     */
    static const ::inflection::util::ULocale& US_SPANISH();
    /**
     * es_CL: Spanish (Chile)
     */
    static const ::inflection::util::ULocale& CHILEAN_SPANISH();
    /**
     * et: Estonian
     */
    static const ::inflection::util::ULocale& ESTONIAN();
    /**
     * et_EE: Estonian (Estonia)
     */
    static const ::inflection::util::ULocale& ESTONIA();
    /**
     * fi: Finnish
     */
    static const ::inflection::util::ULocale& FINNISH();
    /**
     * fi_FI: Finnish (Finland)
     */
    static const ::inflection::util::ULocale& FINLAND();
    /**
     * fr: French
     */
    static const ::inflection::util::ULocale& FRENCH();
    /**
     * fr_FR: French (France)
     */
    static const ::inflection::util::ULocale& FRANCE();
    /**
     * fr_BE: French (Belgium)
     */
    static const ::inflection::util::ULocale& BELGIUM_FRENCH();
    /**
     * fr_CH: French (Switzerland)
     */
    static const ::inflection::util::ULocale& SWITZERLAND_FRENCH();
    /**
     * fr_CA: French (Canada)
     */
    static const ::inflection::util::ULocale& CANADIAN_FRENCH();
    /**
     * he: Hebrew
     */
    static const ::inflection::util::ULocale& HEBREW();
    /**
     * he_IL: Hebrew (Israel)
     */
    static const ::inflection::util::ULocale& ISRAEL();
    /**
     * hi: Hindi
     */
    static const ::inflection::util::ULocale& HINDI();
    /**
     * hi_IN: Hindi (India)
     */
    static const ::inflection::util::ULocale& INDIA_HINDI();
    /**
     * hr: Croatian
     */
    static const ::inflection::util::ULocale& CROATIAN();
    /**
     * hr_HR: Croatian (Croatia)
     */
    static const ::inflection::util::ULocale& CROATIA();
    /**
     * hu: Hungarian
     */
    static const ::inflection::util::ULocale& HUNGARIAN();
    /**
     * hu_HU: Hungarian (Hungary)
     */
    static const ::inflection::util::ULocale& HUNGARY();
    /**
     * id: Indonesian
     */
    static const ::inflection::util::ULocale& INDONESIAN();
    /**
     * id_ID: Indonesian (Indonesia)
     */
    static const ::inflection::util::ULocale& INDONESIA();
    /**
     * is: Icelandic
     */
    static const ::inflection::util::ULocale& ICELANDIC();
    /**
     * is_IS: Icelandic (Iceland)
     */
    static const ::inflection::util::ULocale& ICELAND();
    /**
     * it: Italian
     */
    static const ::inflection::util::ULocale& ITALIAN();
    /**
     * it_IT: Italian (Italy)
     */
    static const ::inflection::util::ULocale& ITALY();
    /**
     * it_CH: Italian (Switzerland)
     */
    static const ::inflection::util::ULocale& SWITZERLAND_ITALIAN();
    /**
     * ja: Japanese
     */
    static const ::inflection::util::ULocale& JAPANESE();
    /**
     * ja_JP: Japanese (Japan)
     */
    static const ::inflection::util::ULocale& JAPAN();
    /**
     * kk: Kazakh
     */
    static const ::inflection::util::ULocale& KAZAKH();
    /**
     * kk_KZ: Kazakh (Kazakhstan)
     */
    static const ::inflection::util::ULocale& KAZAKHSTAN();
    /**
     * ko: Korean
     */
    static const ::inflection::util::ULocale& KOREAN();
    /**
     * ko_KR: Korean (South Korea)
     */
    static const ::inflection::util::ULocale& KOREA();
    /**
     * lt: Lithuanian
     */
    static const ::inflection::util::ULocale& LITHUANIAN();
    /**
     * lt_LT: Lithuanian (Lithuania)
     */
    static const ::inflection::util::ULocale& LITHUANIA();
    /**
     * ms: Malay
     */
    static const ::inflection::util::ULocale& MALAY();
    /**
     * ms_MY: Malay (Malaysia)
     */
    static const ::inflection::util::ULocale& MALAYSIA();
    /**
     * nb: Norwegian Bokmål
     */
    static const ::inflection::util::ULocale& NORWEGIAN();
    /**
     * nb_NO: Norwegian Bokmål (Norway)
     */
    static const ::inflection::util::ULocale& NORWAY();
    /**
     * nl: Dutch
     */
    static const ::inflection::util::ULocale& DUTCH();
    /**
     * nl_NL: Dutch (Netherlands)
     */
    static const ::inflection::util::ULocale& NETHERLANDS();
    /**
     * nl_BE: Dutch (Belgium)
     */
    static const ::inflection::util::ULocale& BELGIUM_DUTCH();
    /**
     * pl: Polish
     */
    static const ::inflection::util::ULocale& POLISH();
    /**
     * pl_PL: Polish (Poland)
     */
    static const ::inflection::util::ULocale& POLAND();
    /**
     * pt: Portuguese
     */
    static const ::inflection::util::ULocale& PORTUGUESE();
    /**
     * pt_BR: Portuguese (Portugal)
     */
    static const ::inflection::util::ULocale& PORTUGAL();
    /**
     * pt_BR: Portuguese (Brazil)
     */
    static const ::inflection::util::ULocale& BRAZIL();
    /**
     * ro: Romanian
     */
    static const ::inflection::util::ULocale& ROMANIAN();
    /**
     * ro_RO: Romanian (Romania)
     */
    static const ::inflection::util::ULocale& ROMANIA();
    /**
     * ru: Russian
     */
    static const ::inflection::util::ULocale& RUSSIAN();
    /**
     * ru_RU: Russian (Russia)
     */
    static const ::inflection::util::ULocale& RUSSIA();
    /**
     * sk: Slovak
     */
    static const ::inflection::util::ULocale& SLOVAK();
    /**
     * sk_SK: Slovak (Slovakia)
     */
    static const ::inflection::util::ULocale& SLOVAKIA();
    /**
     * sr: Serbian
     */
    static const ::inflection::util::ULocale& SERBIAN();
    /**
     * sr_RS: Serbian (Serbia)
     */
    static const ::inflection::util::ULocale& SERBIA();
    /**
     * sv: Swedish
     */
    static const ::inflection::util::ULocale& SWEDISH();
    /**
     * sv_SE: Swedish (Sweden)
     */
    static const ::inflection::util::ULocale& SWEDEN();
    /**
     * th: Thai
     */
    static const ::inflection::util::ULocale& THAI();
    /**
     * th_TH: Thai (Thailand)
     */
    static const ::inflection::util::ULocale& THAILAND();
    /**
     * tr: Turkish
     */
    static const ::inflection::util::ULocale& TURKISH();
    /**
     * tr_TR: Turkish (Turkey)
     */
    static const ::inflection::util::ULocale& TURKEY();
    /**
     * uk: Ukrainian
     */
    static const ::inflection::util::ULocale& UKRAINIAN();
    /**
     * uk_UA: Ukrainian (Ukraine)
     */
    static const ::inflection::util::ULocale& UKRAINE();
    /**
     * vi: Vietnamese
     */
    static const ::inflection::util::ULocale& VIETNAMESE();
    /**
     * vi_VN: Vietnamese (Vietnam)
     */
    static const ::inflection::util::ULocale& VIETNAM();
    /**
     * wuu: Shanghainese
     */
    static const ::inflection::util::ULocale& SHANGHAINESE();
    /**
     * wuu_CN: Shanghainese (China)
     */
    static const ::inflection::util::ULocale& CHINA_SHANGHAINESE();
    /**
     * yue: Cantonese
     */
    static const ::inflection::util::ULocale& CANTONESE();
    /**
     * yue_CN: Cantonese (China)
     * This represents the Cantonese written in the simplified Han script in China.
     */
    static const ::inflection::util::ULocale& CHINA_CANTONESE();
    /**
     * yue_HK: Cantonese (Hong Kong)
     * This represents the Cantonese written in the traditional Han script in Hong Kong.
     */
    static const ::inflection::util::ULocale& HONGKONG_CANTONESE();
    /**
     * zh: Chinese
     * This represents the Chinese macro-language. By default, it represents Simplified Mandarin in China.
     */
    static const ::inflection::util::ULocale& CHINESE();
    /**
     * zh_Hans: Chinese written in the simplified Han script.
     * This represents the Chinese macro-language written in the simplified Han script. By default, it represents Simplified Mandarin.
     */
    static const ::inflection::util::ULocale& SIMPLIFIED_CHINESE();
    /**
     * zh_Hant: Chinese written in the traditional Han script
     * This represents the Chinese macro-language written in the traditional Han script. By default, it represents Traditional Mandarin.
     */
    static const ::inflection::util::ULocale& TRADITIONAL_CHINESE();
    /**
     * zh_HK: Chinese (Hong Kong)
     * This represents the Chinese macro-language written in the traditional Han script in Hong Kong. By default, it represents Traditional Cantonese.
     */
    static const ::inflection::util::ULocale& HONGKONG_CHINESE();
    /**
     * zh_CN: Chinese (China)
     * This represents the Chinese macro-language in China. By default, it represents Simplified Mandarin in China.
     */
    static const ::inflection::util::ULocale& CHINA();
    /**
     * zh_TW: Chinese (Taiwan)
     * This represents the Chinese macro-language in Taiwan. By default, it represents Traditional Mandarin in Taiwan.
     */
    static const ::inflection::util::ULocale& TAIWAN();
    /**
     * The root locale is the locale that all other locales derive their data.
     * It does not represent any particular language or region.
     */
    static const ::inflection::util::ULocale& ROOT();

private:
    static const ::std::map<::std::string, ::std::vector<::inflection::util::ULocale>, std::greater<>>& BASE_SUPPORTED_LOCALES();
};
