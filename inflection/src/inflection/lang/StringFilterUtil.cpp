/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/lang/StringFilterUtil.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/Normalizer2.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/ulocdata.h>
#include <unicode/uscript.h>
#include <map>
#include <mutex>

namespace inflection::lang {

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

const icu4cxx::UnicodeSet& StringFilterUtil::LATIN_SCRIPT()
{
    static auto LATIN_SCRIPT_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[:Latin:]"));
    return *npc(LATIN_SCRIPT_);
}

// Should be called with CLASS_MUTEX locked.
static const icu4cxx::UnicodeSet* getSetSingleton(::icu4cxx::UnicodeSet* set)
{
    // We're caching a set of UnicodeSet objects.
    // We get the approximate set based on size.
    // We iterate over the remaining sets of the same size to get the singleton.
    static auto setSingletonsPtr = new ::std::multimap<int32_t, const ::icu4cxx::UnicodeSet*>();
    auto& setSingletons = *npc(setSingletonsPtr);
    int32_t size = npc(set)->size();
    const auto& [start, end] = setSingletons.equal_range(size);
    for (auto singletonItr = start; singletonItr != end; ++singletonItr) {
        if (npc(singletonItr->second)->equals(*npc(set))) {
            // This is a duplicate. Return the previous value to reuse.
            delete set;
            return singletonItr->second;
        }
    }
    return setSingletons.emplace(size, inflection::util::UnicodeSetUtils::freeze(set))->second;
}

static const icu4cxx::UnicodeSet& getExemplarCharactersWithoutMutex(const ::inflection::util::ULocale& locale) {
    static auto exemplarCachePtr = new ::std::map<::inflection::util::ULocale, const ::icu4cxx::UnicodeSet*>();
    auto& exemplarCache = *npc(exemplarCachePtr);
    auto exemplarSetResult = exemplarCache.find(locale);
    if (exemplarSetResult != exemplarCache.end()) {
        return *npc(exemplarSetResult->second);
    }
    auto exemplarSet = new ::icu4cxx::UnicodeSet();
    auto status = U_ZERO_ERROR;
    auto locData = ulocdata_open(locale.getName().c_str(), &status);
    ulocdata_getExemplarSet(locData, npc(exemplarSet->wrappee_), USET_CASE_INSENSITIVE, ULOCDATA_ES_STANDARD, &status);
    ulocdata_close(locData);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return *npc(exemplarCache.emplace(locale, getSetSingleton(exemplarSet)).first->second);
}

const icu4cxx::UnicodeSet& StringFilterUtil::getExemplarCharacters(const ::inflection::util::ULocale& locale)
{
    std::lock_guard guard(CLASS_MUTEX());
    return getExemplarCharactersWithoutMutex(locale);
}

static bool isNonWhitespaceSeparableCharacter(char32_t cp) {
    auto status = U_ZERO_ERROR;
    const UScriptCode script = uscript_getScript(static_cast<UChar32>(cp), &status);
    return static_cast<bool>(U_SUCCESS(status)) && (static_cast<bool>(uscript_breaksBetweenLetters(script)) || script == USCRIPT_HANGUL);
}

bool StringFilterUtil::hasNonWhitespaceSeparableCharacter(::std::u16string_view str)
{
    char32_t cp;
    const int32_t len = static_cast<int32_t>(str.length());
    for (int32_t idx = 0; idx < len; idx += U16_LENGTH(cp)) {
        cp = ::inflection::util::StringViewUtils::codePointAt(str, idx);
        if (isNonWhitespaceSeparableCharacter(cp)) {
            return true;
        }
    }
    return false;
}

bool StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(::std::u16string_view str)
{
    return !str.empty() && isNonWhitespaceSeparableCharacter(inflection::util::StringViewUtils::codePointAt(str, 0));
}

} // namespace inflection::lang
