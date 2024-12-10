/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/CommonConceptFactory.h>
#include <morphuntion/dialog/InflectableStringConcept.h>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/util/LocaleUtils.h>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/npc.hpp>
#include <string>
#include "util/CFUtils.hpp"


typedef ::morphuntion::util::AutoCRelease<MDSpeakableString*, &mss_destroy> AutoSpeakableStringRelease;
typedef ::morphuntion::util::AutoCRelease<MDNumberConcept*, &mnum_destroy> AutoNumberconceptRelease;
typedef ::morphuntion::util::AutoCRelease<MDInflectableStringConcept*, &minf_destroy> AutoInflectableStringConceptRelease;

static void checkForSuccess(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error == nullptr);
}

static void checkForFailure(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error != nullptr);
    CFRelease(*error);
    *error = nullptr;
}

const MDCommonConceptFactory* getCommonConceptFactory(CFLocaleRef language)
{
    CFErrorRef error = nullptr;
    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, language, &error);
    checkForSuccess(&error);
    return ccf;
}

const MDSemanticFeatureModel* getSemanticFeatureModel(const MDCommonConceptFactory* conceptFactory)
{
    CFErrorRef error = nullptr;
    auto model = mccf_getSemanticFeatureModel(conceptFactory, &error);
    checkForSuccess(&error);
    return model;
}

MDInflectableStringConcept* toInflectableStringConcept(const MDCommonConceptFactory* conceptFactory, const MDSpeakableString* phrase)
{
    CFErrorRef error = nullptr;
    auto result = minf_create(getSemanticFeatureModel(conceptFactory), phrase, &error);
    checkForSuccess(&error);
    return result;
}

static void compareSpeakableStrings(const MDSpeakableString* expected, const MDSpeakableString* actual) {
    auto expectedDesciptiveString = ::morphuntion::util::StringUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::morphuntion::util::StringUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::morphuntion::dialog::SpeakableString*) expected) == *((const ::morphuntion::dialog::SpeakableString*) actual));
}

static void assertQuantity(const MDCommonConceptFactory* conceptFactory, const MDSpeakableString* expected, int64_t count, const MDSemanticFeatureConcept* fromPhrase)
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> language(mccf_getLanguageCopy(conceptFactory, &error));
    morphuntion::util::AutoCFRelease<CFLocaleRef> regionLocale(mloc_createValidRegionAwareLanguage(language.value, language.value));
    AutoNumberconceptRelease numberConcept(mnum_createFromInt64(count, language.value, regionLocale.value, &error));
    checkForSuccess(&error);
    AutoSpeakableStringRelease actualSpeakableString(mccf_quantifyCopy(conceptFactory, numberConcept.value, fromPhrase, &error));
    checkForSuccess(&error);

    compareSpeakableStrings(expected, actualSpeakableString.value);
}

static void assertQuantity(const MDCommonConceptFactory* conceptFactory, const MDSpeakableString* expected, int64_t count, const MDSpeakableString* from)
{
    CFErrorRef error = nullptr;
    AutoInflectableStringConceptRelease fromPhrase(toInflectableStringConcept(conceptFactory, from));
    checkForSuccess(&error);
    auto fromPhraseSemanticFeatureConcept = minf_toSemanticFeatureConcept(fromPhrase.value, &error);
    checkForSuccess(&error);
    assertQuantity(conceptFactory, expected, count, fromPhraseSemanticFeatureConcept);
}

static void assertQuantity(const MDCommonConceptFactory* conceptFactory, const MDSpeakableString* expected, int64_t count, const MDSpeakableString* from, CFStringRef caseString)
{
    CFErrorRef error = nullptr;
    AutoInflectableStringConceptRelease fromPhrase(toInflectableStringConcept(conceptFactory, from));
    checkForSuccess(&error);

    auto fromPhraseSemanticFeatureConcept = minf_toSemanticFeatureConcept(fromPhrase.value, &error);
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> caseStr(util::CFUtils::to_CFString(u"case"));
    msfc_putConstraintByName(fromPhraseSemanticFeatureConcept, caseStr.value, caseString, &error);
    checkForSuccess(&error);

    assertQuantity(conceptFactory, expected, count, fromPhraseSemanticFeatureConcept);
}

static void runBasicTestCase(const MDCommonConceptFactory* conceptFactory,  const ::std::vector<::std::tuple<const char16_t*, const char16_t*, int64_t, const char16_t*>>& testCases) {
    for (const auto &testCase: testCases) {
        AutoSpeakableStringRelease expected((MDSpeakableString *) new ::morphuntion::dialog::SpeakableString(::std::get<0>(testCase), ::std::get<1>(testCase)));
        AutoSpeakableStringRelease fromPhrase((MDSpeakableString *) new ::morphuntion::dialog::SpeakableString(::std::get<3>(testCase)));
        assertQuantity(conceptFactory, expected.value, ::std::get<2>(testCase), fromPhrase.value);
    }
}

static void runTestCaseWithCaseString(const MDCommonConceptFactory* conceptFactory,  const ::std::vector<::std::tuple<const char16_t*, const char16_t*, int64_t, const char16_t*, const char16_t*>>& testCases) {
    for (const auto &testCase: testCases) {
        AutoSpeakableStringRelease expected((MDSpeakableString *) new ::morphuntion::dialog::SpeakableString(::std::get<0>(testCase), ::std::get<1>(testCase)));
        AutoSpeakableStringRelease fromPhrase((MDSpeakableString *) new ::morphuntion::dialog::SpeakableString(::std::get<3>(testCase)));
        morphuntion::util::AutoCFRelease<CFStringRef> caseString(::util::CFUtils::to_CFString(::std::get<4>(testCase)));
        assertQuantity(conceptFactory, expected.value, ::std::get<2>(testCase), fromPhrase.value, caseString.value);
    }
}

TEST_CASE("QuantifyTest-c#testEnglish")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 yen", u"1 yen", 1, u"yen"),
            ::std::make_tuple(u"2 yen", u"2 yen", 2, u"yen"),
            ::std::make_tuple(u"1 kilometer", u"1 kilometer", 1, u"kilometer"),
            ::std::make_tuple(u"2 kilometers", u"2 kilometers", 2, u"kilometer")
    });
}

TEST_CASE("QuantifyTest-c#testFailure")
{
    CFErrorRef error = nullptr;
    //Empty locale throws exception
    mnum_createFromInt64(0, nullptr, nullptr, &error);
    checkForFailure(&error);

    mnum_createFromDouble(0, nullptr, nullptr, &error);
    checkForFailure(&error);

    mccf_getLanguageCopy(nullptr, &error);
    checkForFailure(&error);

    mnum_toStringCopy(nullptr, &error);
    checkForFailure(&error);

    mnum_asWordsCopy(nullptr, &error);
    checkForFailure(&error);
    mnum_asWordsVariantCopy(nullptr, nullptr, &error);
    checkForFailure(&error);

    mnum_asSpokenWordsCopy(nullptr, &error);
    checkForFailure(&error);
    mnum_asSpokenWordsVariantCopy(nullptr, nullptr, &error);
    checkForFailure(&error);

    mnum_asDigitsCopy(nullptr, &error);
    checkForFailure(&error);

    mnum_asOrdinalDigitsCopy(nullptr, &error);
    checkForFailure(&error);

    mnum_asOrginalDigitsVariantCopy(nullptr, nullptr, &error);
    checkForFailure(&error);

    mnum_clone(nullptr, &error);
    checkForFailure(&error);

    mccf_quantifyCopy(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);

    mccf_quantifyFormattedCopy(nullptr, nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
}

TEST_CASE("QuantifyTest-c#testFrench")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::FRENCH()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 homme", u"un homme", 1, u"homme"),
            ::std::make_tuple(u"2 hommes", u"deux hommes", 2, u"homme"),
            ::std::make_tuple(u"1 femme", u"une femme", 1, u"femme"),
            ::std::make_tuple(u"2 femmes", u"deux femmes", 2, u"femme")
    });
}

TEST_CASE("QuantifyTest-c#testPortuguese")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::PORTUGUESE()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 homem", u"um homem", 1, u"homem"),
            ::std::make_tuple(u"2 homens", u"dois homens", 2, u"homem"),
            ::std::make_tuple(u"1 mulheraça", u"uma mulheraça", 1, u"mulheraça"),
            ::std::make_tuple(u"2 mulheraças", u"duas mulheraças", 2, u"mulheraça")
    });
}

TEST_CASE("QuantifyTest-c#testSpanish")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SPANISH()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 nota", u"una nota", 1, u"nota"),
            ::std::make_tuple(u"2 notas", u"dos notas", 2, u"nota"),
            ::std::make_tuple(u"1 recordatorio", u"un recordatorio", 1, u"recordatorio"),
            ::std::make_tuple(u"2 recordatorios", u"dos recordatorios", 2, u"recordatorio"),
            ::std::make_tuple(u"1 paraguas", u"un paraguas", 1, u"paraguas"),
            ::std::make_tuple(u"2 paraguas", u"dos paraguas", 2, u"paraguas")
    });
}

TEST_CASE("QuantifyTest-c#testItalian")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ITALIAN()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"0 settimane", u"zero settimane", 0, u"settimana"),
            ::std::make_tuple(u"una settimana", u"una settimana", 1, u"settimana"),
            ::std::make_tuple(u"2 settimane", u"due settimane", 2, u"settimana"),
            ::std::make_tuple(u"3 settimane", u"tre settimane", 3, u"settimana"),
            ::std::make_tuple(u"0 metri", u"zero metri", 0, u"metro"),
            ::std::make_tuple(u"un metro", u"un metro", 1, u"metro"),
            ::std::make_tuple(u"2 metri", u"due metri", 2, u"metro"),
            ::std::make_tuple(u"3 metri", u"tre metri", 3, u"metro"),
            ::std::make_tuple(u"0 ore", u"zero ore", 0, u"ora"),
            ::std::make_tuple(u"un’ora", u"un’ora", 1, u"ora"),
            ::std::make_tuple(u"2 ore", u"due ore", 2, u"ora"),
            ::std::make_tuple(u"3 ore", u"tre ore", 3, u"ora"),
            ::std::make_tuple(u"0 yen", u"zero yen", 0, u"yen"),
            ::std::make_tuple(u"uno yen", u"uno yen", 1, u"yen"),
            ::std::make_tuple(u"2 yen", u"due yen", 2, u"yen"),
            ::std::make_tuple(u"3 yen", u"tre yen", 3, u"yen")
    });
}

TEST_CASE("QuantifyTest-c#testGerman")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::GERMAN()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);
    runBasicTestCase(conceptFactory, {
        ::std::make_tuple(u"1 Kind", u"ein Kind", 1, u"Kind"),
        ::std::make_tuple(u"2 Kinder", u"zwei Kinder", 2, u"Kind"),
        ::std::make_tuple(u"1 Film", u"ein Film", 1, u"Film"),
        ::std::make_tuple(u"2 Filme", u"zwei Filme", 2, u"Film"),
        ::std::make_tuple(u"1 Antwort", u"eine Antwort", 1, u"Antwort"),
        ::std::make_tuple(u"2 Antworten", u"zwei Antworten", 2, u"Antwort")
    });

    runTestCaseWithCaseString(conceptFactory, {
        ::std::make_tuple(u"1 Kindes", u"eines Kindes", 1, u"Kind", u"genitive"),
        ::std::make_tuple(u"2 Kinder", u"zwei Kinder", 2, u"Kind", u"genitive"),
        ::std::make_tuple(u"1 Kind", u"einem Kind", 1, u"Kind", u"dative"),
        ::std::make_tuple(u"2 Kindern", u"zwei Kindern", 2, u"Kind", u"dative"),
        ::std::make_tuple(u"1 Kind", u"ein Kind", 1, u"Kind", u"accusative"),
        ::std::make_tuple(u"2 Kinder", u"zwei Kinder", 2, u"Kind", u"accusative"),
        ::std::make_tuple(u"1 Films", u"eines Films", 1, u"Film", u"genitive"),
        ::std::make_tuple(u"2 Filme", u"zwei Filme", 2, u"Film", u"genitive"),
        ::std::make_tuple(u"1 Film", u"einem Film", 1, u"Film", u"dative"),
        ::std::make_tuple(u"2 Filmen", u"zwei Filmen", 2, u"Film", u"dative"),
        ::std::make_tuple(u"1 Film", u"einen Film", 1, u"Film", u"accusative"),
        ::std::make_tuple(u"2 Filme", u"zwei Filme", 2, u"Film", u"accusative"),
        ::std::make_tuple(u"1 Antwort", u"einer Antwort", 1, u"Antwort", u"genitive"),
        ::std::make_tuple(u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"genitive"),
        ::std::make_tuple(u"1 Antwort", u"einer Antwort", 1, u"Antwort", u"dative"),
        ::std::make_tuple(u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"dative"),
        ::std::make_tuple(u"1 Antwort", u"eine Antwort", 1, u"Antwort", u"accusative"),
        ::std::make_tuple(u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"accusative")
    });
}

TEST_CASE("QuantifyTest-c#testExplicitNumber")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));
    auto const conceptFactory = getCommonConceptFactory(cfLocale.value);

    morphuntion::util::AutoCFRelease<CFStringRef> note(util::CFUtils::to_CFString(u"note"));
    AutoSpeakableStringRelease phraseStr(mss_create(note.value, &error));
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> aCoupeOf(util::CFUtils::to_CFString(u"a couple of"));
    AutoSpeakableStringRelease coupleStr(mss_create(aCoupeOf.value, &error));
    checkForSuccess(&error);

    AutoInflectableStringConceptRelease phrase(minf_create(getSemanticFeatureModel(conceptFactory), phraseStr.value, &error));
    checkForSuccess(&error);

    auto phraseConcept = minf_toSemanticFeatureConcept(phrase.value, &error);

    AutoNumberconceptRelease numberConcept(mnum_createFromInt64(2, cfLocale.value, cfLocale.value, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease actualSpeakableString(mccf_quantifyFormattedCopy(conceptFactory, numberConcept.value, coupleStr.value, phraseConcept, &error));
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> printedResult(mss_createPrintCopy(actualSpeakableString.value, &error));
    checkForSuccess(&error);

    CHECK("a couple of notes" == ::util::CFUtils::to_string(printedResult.value));
}
