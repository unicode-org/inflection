/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/CommonConceptFactory.h>
#include <inflection/dialog/InflectableStringConcept.h>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/npc.hpp>
#include <string>
#include "util/StringContainer.hpp"

typedef ::inflection::util::AutoCRelease<IDSpeakableString*, &iss_destroy> AutoSpeakableStringRelease;
typedef ::inflection::util::AutoCRelease<IDNumberConcept*, &inum_destroy> AutoNumberconceptRelease;
typedef ::inflection::util::AutoCRelease<IDInflectableStringConcept*, &iinf_destroy> AutoInflectableStringConceptRelease;

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

const IDCommonConceptFactory* getCommonConceptFactory(const char* language)
{
    auto error = U_ZERO_ERROR;
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, language, &error);
    REQUIRE(U_SUCCESS(error));
    return ccf;
}

const IDSemanticFeatureModel* getSemanticFeatureModel(const IDCommonConceptFactory* conceptFactory)
{
    auto error = U_ZERO_ERROR;
    auto model = iccf_getSemanticFeatureModel(conceptFactory, &error);
    REQUIRE(U_SUCCESS(error));
    return model;
}

IDInflectableStringConcept* toInflectableStringConcept(const IDCommonConceptFactory* conceptFactory, const IDSpeakableString* phrase)
{
    auto error = U_ZERO_ERROR;
    auto result = iinf_create(getSemanticFeatureModel(conceptFactory), phrase, &error);
    REQUIRE(U_SUCCESS(error));
    return result;
}

static void compareSpeakableStrings(const IDSpeakableString* expected, const IDSpeakableString* actual) {
    auto expectedDesciptiveString = ::inflection::util::StringUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::inflection::util::StringUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::inflection::dialog::SpeakableString*) expected) == *((const ::inflection::dialog::SpeakableString*) actual));
}

static void assertQuantity(const IDCommonConceptFactory* conceptFactory, const IDSpeakableString* expected, int64_t count, const IDSemanticFeatureConcept* fromPhrase)
{
    auto error = U_ZERO_ERROR;
    auto language(iccf_getLanguage(conceptFactory, &error));
    AutoNumberconceptRelease numberConcept(inum_createFromInt64(count, language, language, &error));
    REQUIRE(U_SUCCESS(error));
    AutoSpeakableStringRelease actualSpeakableString(iccf_quantifyCopy(conceptFactory, numberConcept.value, fromPhrase, &error));
    REQUIRE(U_SUCCESS(error));

    compareSpeakableStrings(expected, actualSpeakableString.value);
}

static void assertQuantity(const IDCommonConceptFactory* conceptFactory, const IDSpeakableString* expected, int64_t count, const IDSpeakableString* from)
{
    auto error = U_ZERO_ERROR;
    AutoInflectableStringConceptRelease fromPhrase(toInflectableStringConcept(conceptFactory, from));
    REQUIRE(U_SUCCESS(error));
    auto fromPhraseSemanticFeatureConcept = iinf_toSemanticFeatureConcept(fromPhrase.value, &error);
    REQUIRE(U_SUCCESS(error));
    assertQuantity(conceptFactory, expected, count, fromPhraseSemanticFeatureConcept);
}

static void assertQuantity(const IDCommonConceptFactory* conceptFactory, const IDSpeakableString* expected, int64_t count, const IDSpeakableString* from, const char16_t* caseString)
{
    auto error = U_ZERO_ERROR;
    AutoInflectableStringConceptRelease fromPhrase(toInflectableStringConcept(conceptFactory, from));
    REQUIRE(U_SUCCESS(error));

    auto fromPhraseSemanticFeatureConcept = iinf_toSemanticFeatureConcept(fromPhrase.value, &error);
    REQUIRE(U_SUCCESS(error));

    isfc_putConstraintByName(fromPhraseSemanticFeatureConcept, u"case", caseString, -1, &error);
    REQUIRE(U_SUCCESS(error));

    assertQuantity(conceptFactory, expected, count, fromPhraseSemanticFeatureConcept);
}

static void runBasicTestCase(const IDCommonConceptFactory* conceptFactory,  const ::std::vector<::std::tuple<const char16_t*, const char16_t*, int64_t, const char16_t*>>& testCases) {
    for (const auto &testCase: testCases) {
        AutoSpeakableStringRelease expected((IDSpeakableString *) new ::inflection::dialog::SpeakableString(::std::get<0>(testCase), ::std::get<1>(testCase)));
        AutoSpeakableStringRelease fromPhrase((IDSpeakableString *) new ::inflection::dialog::SpeakableString(::std::get<3>(testCase)));
        assertQuantity(conceptFactory, expected.value, ::std::get<2>(testCase), fromPhrase.value);
    }
}

static void runTestCaseWithCaseString(const IDCommonConceptFactory* conceptFactory,  const ::std::vector<::std::tuple<const char16_t*, const char16_t*, int64_t, const char16_t*, const char16_t*>>& testCases) {
    for (const auto &testCase: testCases) {
        AutoSpeakableStringRelease expected((IDSpeakableString *) new ::inflection::dialog::SpeakableString(::std::get<0>(testCase), ::std::get<1>(testCase)));
        AutoSpeakableStringRelease fromPhrase((IDSpeakableString *) new ::inflection::dialog::SpeakableString(::std::get<3>(testCase)));
        assertQuantity(conceptFactory, expected.value, ::std::get<2>(testCase), fromPhrase.value, ::std::get<4>(testCase));
    }
}

TEST_CASE("QuantifyTest-c#testEnglish")
{
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 yen", u"1 yen", 1, u"yen"),
            ::std::make_tuple(u"2 yen", u"2 yen", 2, u"yen"),
            ::std::make_tuple(u"1 kilometer", u"1 kilometer", 1, u"kilometer"),
            ::std::make_tuple(u"2 kilometers", u"2 kilometers", 2, u"kilometer")
    });
}

TEST_CASE("QuantifyTest-c#testFailure")
{
    auto error = U_ZERO_ERROR;
    //Empty locale throws exception
    inum_createFromInt64(0, nullptr, nullptr, &error);
    checkForFailure(&error);

    inum_createFromDouble(0, nullptr, nullptr, &error);
    checkForFailure(&error);

    iccf_getLanguage(nullptr, &error);
    checkForFailure(&error);

    inum_toString(nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    inum_asWords(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    inum_asWordsVariant(nullptr, nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    inum_asSpokenWordsCopy(nullptr, &error);
    checkForFailure(&error);
    inum_asSpokenWordsVariantCopy(nullptr, nullptr, &error);
    checkForFailure(&error);

    inum_asDigitsCopy(nullptr, &error);
    checkForFailure(&error);

    inum_asOrdinalDigits(nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    inum_asOrginalDigitsVariant(nullptr, nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    inum_clone(nullptr, &error);
    checkForFailure(&error);

    iccf_quantifyCopy(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);

    iccf_quantifyFormattedCopy(nullptr, nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
}

TEST_CASE("QuantifyTest-c#testFrench")
{
    auto locale = ::inflection::util::LocaleUtils::FRENCH().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 homme", u"un homme", 1, u"homme"),
            ::std::make_tuple(u"2 hommes", u"deux hommes", 2, u"homme"),
            ::std::make_tuple(u"1 femme", u"une femme", 1, u"femme"),
            ::std::make_tuple(u"2 femmes", u"deux femmes", 2, u"femme")
    });
}

TEST_CASE("QuantifyTest-c#testPortuguese")
{
    auto locale = ::inflection::util::LocaleUtils::PORTUGUESE().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
    runBasicTestCase(conceptFactory, {
            ::std::make_tuple(u"1 homem", u"um homem", 1, u"homem"),
            ::std::make_tuple(u"2 homens", u"dois homens", 2, u"homem"),
            ::std::make_tuple(u"1 mulheraça", u"uma mulheraça", 1, u"mulheraça"),
            ::std::make_tuple(u"2 mulheraças", u"duas mulheraças", 2, u"mulheraça")
    });
}

TEST_CASE("QuantifyTest-c#testSpanish")
{
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
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
    auto locale = ::inflection::util::LocaleUtils::ITALIAN().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
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
    auto locale = ::inflection::util::LocaleUtils::GERMAN().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);
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
        ::std::make_tuple(u"1 Filmes", u"eines Filmes", 1, u"Film", u"genitive"),
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
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();
    auto const conceptFactory = getCommonConceptFactory(locale);

    AutoSpeakableStringRelease phraseStr(iss_create(u"note", -1, &error));
    REQUIRE(U_SUCCESS(error));

    AutoSpeakableStringRelease coupleStr(iss_create(u"a couple of", -1, &error));
    REQUIRE(U_SUCCESS(error));

    AutoInflectableStringConceptRelease phrase(iinf_create(getSemanticFeatureModel(conceptFactory), phraseStr.value, &error));
    REQUIRE(U_SUCCESS(error));

    auto phraseConcept = iinf_toSemanticFeatureConcept(phrase.value, &error);

    AutoNumberconceptRelease numberConcept(inum_createFromInt64(2, locale, locale, &error));
    REQUIRE(U_SUCCESS(error));

    AutoSpeakableStringRelease actualSpeakableString(iccf_quantifyFormattedCopy(conceptFactory, numberConcept.value, coupleStr.value, phraseConcept, &error));
    REQUIRE(U_SUCCESS(error));

    util::StringContainer<IDSpeakableString, iss_getPrint> printedResult(actualSpeakableString.value);
    REQUIRE(U_SUCCESS(error));

    CHECK("a couple of notes" == ::inflection::util::StringUtils::to_string(printedResult.value));
}
