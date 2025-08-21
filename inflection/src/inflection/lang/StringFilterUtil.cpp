/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/lang/StringFilterUtil.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/Normalizer2.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/ulocdata.h>
#include <map>
#include <mutex>

namespace inflection::lang {

const icu4cxx::UnicodeSet& StringFilterUtil::LATIN_SCRIPT()
{
    static auto LATIN_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Latin:]"));
    return *npc(LATIN_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::CYRILLIC_SCRIPT()
{
    static auto CYRILLIC_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Cyrillic:]"));
    return *npc(CYRILLIC_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::ARABIC_SCRIPT()
{
    static auto ARABIC_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Arabic:]"));
    return *npc(ARABIC_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::HEBREW_SCRIPT()
{
    static auto HEBREW_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Hebrew:]"));
    return *npc(HEBREW_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::MALAYALAM_SCRIPT()
{
    static auto MALAYALAM_SCRIPT_ =
        ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Malayalam:]"));
    return *npc(MALAYALAM_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::HAN_SCRIPT()
{
    static auto HAN_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Han:]"));
    return *npc(HAN_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::HANGUL_SCRIPT()
{
    static auto HANGUL_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Hangul:]"));
    return *npc(HANGUL_SCRIPT_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::NON_WHITESPACE_SEPARATED_SCRIPTS()
{
    static auto NON_WHITESPACE_SEPARATED_SCRIPTS_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[\\p{Hangul}\\p{Hiragana}\\p{Katakana}\\p{Han}\\p{Thai}]"));
    return *npc(NON_WHITESPACE_SEPARATED_SCRIPTS_);
}

static const icu4cxx::Normalizer2& NFD()
{
    static auto NFD_ = new ::icu4cxx::Normalizer2(::icu4cxx::Normalizer2::getNFDInstance());
    return *npc(NFD_);
}

static const icu4cxx::Normalizer2& NFC()
{
    static auto NFC_ = new ::icu4cxx::Normalizer2(::icu4cxx::Normalizer2::getNFCInstance());
    return *npc(NFC_);
}

::std::u16string StringFilterUtil::filteredUnaccent(const ::icu4cxx::UnicodeSet& foreignSet, const ::icu4cxx::UnicodeSet& setToRemove, const ::std::u16string& str)
{
    char32_t cp;
    int32_t strLen = int32_t(str.length());
    bool needsInitialization = true;
    ::std::u16string stringBuilder;
    for (int32_t i = 0; i < strLen; i += U16_LENGTH(cp)) {
        cp = ::inflection::util::StringViewUtils::codePointAt(str, i);
        if (foreignSet.contains(cp)) {
            if (needsInitialization) {
                needsInitialization = false;
                stringBuilder.assign(str, 0, i);
            }
            ::std::u16string codePoint;
            ::inflection::util::StringUtils::appendCodePoint(&codePoint, cp);
            ::std::u16string stripped;
            ::inflection::util::UnicodeSetUtils::removeSetFromString(&stripped, setToRemove, ::inflection::dictionary::PhraseProperties::normalize(NFD(), codePoint));
            stringBuilder.append(stripped.length() > 1 ? ::inflection::dictionary::PhraseProperties::normalize(NFC(), stripped) : stripped);
        } else if (!stringBuilder.empty()) {
            ::inflection::util::StringUtils::appendCodePoint(&stringBuilder, cp);
        }
    }
    if (!needsInitialization) {
        return stringBuilder;
    }
    return str;
}

const icu4cxx::UnicodeSet& StringFilterUtil::NFD_VOLATILE_CHARACTERS()
{
    static auto NFD_VOLATILE_CHARACTERS_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Mark:]]-[:NFD_Inert:]]"));
    return *npc(NFD_VOLATILE_CHARACTERS_);
}

const icu4cxx::UnicodeSet& StringFilterUtil::COMBINING_DIACRITICAL_MARKS()
{
    static auto COMBINING_DIACRITICAL_MARKS_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[:Nonspacing_Mark:]&[:Zinh:]]"));
    return *npc(COMBINING_DIACRITICAL_MARKS_);
}

static ::std::map<const ::icu4cxx::UnicodeSet*, const ::icu4cxx::UnicodeSet*>* SET_SINGLETONS()
{
    static auto setSingletons_ = new ::std::map<const ::icu4cxx::UnicodeSet*, const ::icu4cxx::UnicodeSet*>();
    return setSingletons_;
}

const icu4cxx::UnicodeSet* StringFilterUtil::getSetSingleton(::icu4cxx::UnicodeSet* input)
{
    // TODO: this code probably doesn't work that well anymore. We should check the caching again.
    auto setSingletons = SET_SINGLETONS();
    const icu4cxx::UnicodeSet* singleton;
    ::std::unique_ptr<::icu4cxx::UnicodeSet> set(input);
    auto singletonCache = npc(setSingletons)->find(set.get());
    if (singletonCache == npc(setSingletons)->end()) {
        singleton = ::inflection::util::UnicodeSetUtils::freeze(set.release());
        npc(setSingletons)->emplace(singleton, singleton);
    }
    else {
        singleton = singletonCache->second;
    }
    return singleton;
}

const icu4cxx::UnicodeSet& StringFilterUtil::getExemplarCharacters(const ::inflection::util::ULocale& locale)
{
    static auto functionMutex = new std::mutex();
    std::lock_guard<std::mutex> guard(*npc(functionMutex));
    static auto exemplarCache = new ::std::map<::inflection::util::ULocale, const ::icu4cxx::UnicodeSet*>();
    auto exemplarSetResult = npc(exemplarCache)->find(locale);
    if (exemplarSetResult != npc(exemplarCache)->end()) {
        return *npc(exemplarSetResult->second);
    }
    auto exemplarSet = new ::icu4cxx::UnicodeSet();
    auto status = U_ZERO_ERROR;
    auto locData = ulocdata_open(locale.getName().c_str(), &status);
    ulocdata_getExemplarSet(locData, exemplarSet->wrappee_, USET_CASE_INSENSITIVE, ULOCDATA_ES_STANDARD, &status);
    ulocdata_close(locData);
    ::inflection::exception::ICUException::throwOnFailure(status);
    auto constExemplarSet = getSetSingleton(exemplarSet);
    npc(exemplarCache)->emplace(locale, constExemplarSet);
    return *npc(constExemplarSet);
}

static ::std::map<::inflection::util::ULocale, const ::icu4cxx::UnicodeSet*>* NfdVolatileForeignExemplarCache()
{
    static auto NfdVolatileForeignExemplarCache_ = new ::std::map<::inflection::util::ULocale, const ::icu4cxx::UnicodeSet*>();
    return NfdVolatileForeignExemplarCache_;
}

const icu4cxx::UnicodeSet& StringFilterUtil::getNfdVolatileForeignExemplarCharacters(const ::inflection::util::ULocale& locale)
{
    static auto functionMutex = new std::mutex();
    std::lock_guard<std::mutex> guard(*npc(functionMutex));
    auto nfdVolatileForeignExemplarCache = NfdVolatileForeignExemplarCache();
    auto exemplarSet = npc(nfdVolatileForeignExemplarCache)->find(locale);
    if (exemplarSet != nfdVolatileForeignExemplarCache->end()) {
        return *npc(exemplarSet->second);
    }
    auto nfdVolatileCharacters = NFD_VOLATILE_CHARACTERS().cloneAsThawed();
    nfdVolatileCharacters.removeAll(getExemplarCharacters(locale));
    auto constExemplarSet = getSetSingleton(new icu4cxx::UnicodeSet(nfdVolatileCharacters));
    npc(nfdVolatileForeignExemplarCache)->emplace(locale, constExemplarSet);
    return *npc(constExemplarSet);
}

::std::u16string StringFilterUtil::stripNonNativeAccents(const ::inflection::util::ULocale& locale, const ::std::u16string& str)
{
    return filteredUnaccent(getNfdVolatileForeignExemplarCharacters(locale), COMBINING_DIACRITICAL_MARKS(), str);
}

static const UNormalizer2* getNFDInstance() {
    auto status = U_ZERO_ERROR;
    auto retVal = unorm2_getNFDInstance(&status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return retVal;
}
static const UNormalizer2* NFD_NORMALIZER()
{
    static auto NFD_NORMALIZER_ = getNFDInstance();
    return NFD_NORMALIZER_;
}

::std::u16string* StringFilterUtil::unaccent(::std::u16string* dest, ::std::u16string_view s)
{
    auto status = U_ZERO_ERROR;
    int32_t length = int32_t(s.length());
    ::std::u16string normalizedResult;
    if (length > 0) {
        normalizedResult.resize(length, 0); // Typically, we're not changing the size.
        length = unorm2_normalize(NFD_NORMALIZER(), (const UChar *)s.data(), (int32_t)s.length(), (UChar *)&normalizedResult[0], length, &status);
        if (length != int32_t(normalizedResult.length())) {
            normalizedResult.resize(length, 0);
        }
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            status = U_ZERO_ERROR;
            unorm2_normalize(NFD_NORMALIZER(), (const UChar *)s.data(), (int32_t)s.length(), (UChar *)&normalizedResult[0], length, &status);
        }
        ::inflection::exception::ICUException::throwOnFailure(status);
    }
    inflection::util::UnicodeSetUtils::removeSetFromString(dest, COMBINING_DIACRITICAL_MARKS(), normalizedResult);
    return dest;
}

bool StringFilterUtil::hasAccents(::std::u16string_view s)
{
    ::std::u16string s1Str;
    return s != *unaccent(&s1Str, s);
}

bool StringFilterUtil::equalIgnoreCaseAndAllAccents(::std::u16string_view s1, ::std::u16string_view s2, const ::inflection::util::ULocale& locale)
{
    ::std::u16string s1Str;
    ::std::u16string s2Str;
    inflection::util::StringViewUtils::lowercase(&s1Str, s1, locale);
    inflection::util::StringViewUtils::lowercase(&s2Str, s2, locale);
    ::std::u16string s1Unaccented;
    ::std::u16string s2Unaccented;
    unaccent(&s1Unaccented, s1Str);
    unaccent(&s2Unaccented, s2Str);
    return s1Unaccented == s2Unaccented;
}

bool StringFilterUtil::hasNonWhitespaceSeparableCharacter(::std::u16string_view str)
{
    return !str.empty() && inflection::util::UnicodeSetUtils::containsSome(NON_WHITESPACE_SEPARATED_SCRIPTS(), str);
}

bool StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(::std::u16string_view str)
{
    return !str.empty() && NON_WHITESPACE_SEPARATED_SCRIPTS().contains(::inflection::util::StringViewUtils::codePointAt(str, 0));
}

} // namespace inflection::lang
