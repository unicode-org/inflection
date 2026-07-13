/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/CommonConceptFactory.hpp>
#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>
#include <string>
#include <utility>

const inflection::dialog::CommonConceptFactory* getCommonConceptFactory(const ::inflection::util::ULocale& language)
{
    return npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(language);
}

inflection::dialog::InflectableStringConcept* toInflectableStringConcept(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& phrase)
{
    return new inflection::dialog::InflectableStringConcept(npc(conceptFactory)->getSemanticFeatureModel(), phrase);
}

inflection::dialog::InflectableStringConcept* toInflectableStringConcept(const inflection::dialog::CommonConceptFactory* conceptFactory, const std::u16string& phrase)
{
    return new inflection::dialog::InflectableStringConcept(npc(conceptFactory)->getSemanticFeatureModel(), ::inflection::dialog::SpeakableString(phrase));
}

template<typename T>
static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& expected, T count, const inflection::dialog::SemanticFeatureConceptBase* fromPhrase)
{
    ::std::unique_ptr<::inflection::dialog::NumberConcept> value(npc(std::is_same_v<T, double>
            ? npc(conceptFactory)->number(npc(conceptFactory)->getLanguage(), (double)count)
            : npc(conceptFactory)->number(npc(conceptFactory)->getLanguage(), (int64_t)count)));
    ::std::unique_ptr<::inflection::dialog::SpeakableString> result(npc(npc(conceptFactory)->quantify(*value, fromPhrase)));
    INFO(::std::string("expected=") + ::inflection::util::StringUtils::to_string(expected.toString()) + " result=" + ::inflection::util::StringUtils::to_string(result->toString()));
    CHECK(expected == *result);
}

template<typename T>
static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& expected, T count, const ::inflection::dialog::SpeakableString& from)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, from)));
    assertQuantity(conceptFactory, expected, count, fromPhrase.get());
}

template<typename T>
static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const std::u16string& expected, T count, const ::std::u16string& from)
{
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(expected), count, ::inflection::dialog::SpeakableString(from));
}

template<typename T>
static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const std::u16string& expectedPrint, const std::u16string& expectedSpeak, T count, const std::u16string& from)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, from)));
    inflection::dialog::SpeakableString expected(expectedPrint, expectedSpeak);
    assertQuantity(conceptFactory, expected, count, fromPhrase.get());
}

template<typename T>
static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const std::u16string& expectedPrint, const std::u16string& expectedSpeak, T count, const std::u16string& from, const std::u16string& caseString)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, from)));
    auto caseFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"case");
    fromPhrase->putConstraint(*npc(caseFeature), caseString);
    inflection::dialog::SpeakableString expected(expectedPrint, expectedSpeak);
    assertQuantity(conceptFactory, expected, count, fromPhrase.get());
}

static void assertQuantityCount(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& expected, int64_t count, const ::inflection::dialog::SpeakableString& from, const std::u16string& caseString, const std::u16string& numberString)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, from)));
    auto caseFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"case");
    auto numberFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"number");
    inflectableConcept->putConstraint(*npc(caseFeature), caseString);
    inflectableConcept->putConstraint(*npc(numberFeature), numberString);
    assertQuantity(conceptFactory, expected, count, inflectableConcept.get());
}

static void assertQuantity(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& expected, int64_t count, const ::inflection::dialog::SpeakableString& from, const std::u16string& caseString)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, from)));
    auto caseFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"case");
    inflectableConcept->putConstraint(*npc(caseFeature), caseString);
    assertQuantity(conceptFactory, expected, count, inflectableConcept.get());
}

static void assertQuantityDefiniteness(const inflection::dialog::CommonConceptFactory* conceptFactory, const ::inflection::dialog::SpeakableString& expected, int64_t count, const ::inflection::dialog::SpeakableString& from, const std::u16string& caseString, const std::u16string& definitenessString)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, from)));
    auto countFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"definiteness");
    inflectableConcept->putConstraint(*npc(countFeature), definitenessString);
    if (!caseString.empty()) {
        auto caseFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"case");
        inflectableConcept->putConstraint(*npc(caseFeature), caseString);
    }
    assertQuantity(conceptFactory, expected, count, inflectableConcept.get());
}

TEST_CASE("QuantifyTest#testEnglish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::US());
    assertQuantity(conceptFactory, u"1 yen", 1, u"yen");
    assertQuantity(conceptFactory, u"2 yen", 2, u"yen");
    assertQuantity(conceptFactory, u"1 kilometer", 1, u"kilometer");
    assertQuantity(conceptFactory, u"2 kilometers", 2, u"kilometer");
}

TEST_CASE("QuantifyTest#testThai")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::THAILAND());
    assertQuantity(conceptFactory, u"restaurant 1", 1, u"restaurant");
    assertQuantity(conceptFactory, u"restaurant 2", 2, u"restaurant");
}

TEST_CASE("QuantifyTest#testKorean")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::KOREA());
    assertQuantity(conceptFactory, u"1 restaurant", 1, u"restaurant");
    assertQuantity(conceptFactory, u"2 restaurant", 2, u"restaurant");
    assertQuantity(conceptFactory, u"3개", 3, u"개");
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::inflection::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2 Word Blah"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testArabic")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::SAUDI_ARABIA());
    assertQuantity(conceptFactory, u"٠ رسالة", u"صفر رسالة", 0, u"رسالة");
    assertQuantity(conceptFactory, u"رسالة", 1, u"رسالة");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"رسالة"), 1, ::inflection::dialog::SpeakableString(u"رسالة"), u"accusative");
    assertQuantity(conceptFactory, u"رسالتان", 2, u"رسالة");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"رسالتين"), 2, ::inflection::dialog::SpeakableString(u"رسالة"), u"accusative");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"رسالتين"), 2, ::inflection::dialog::SpeakableString(u"رسالة"), u"genitive");
    assertQuantity(conceptFactory, u"٣ رسائل", u"ثلاثة رسائل", 3, u"رسالة");
    assertQuantity(conceptFactory, u"١١ رسالة", u"إحدى عشر رسالة", 11, u"رسالة");
    assertQuantity(conceptFactory, u"١٠٠ رسالة", u"مائة رسالة", 100, u"رسالة");

    // reminder
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكير"), 1, ::inflection::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكيرا"), 1, ::inflection::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكير"), 1, ::inflection::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكيران"), 2, ::inflection::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكيرين"), 2, ::inflection::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"تذكيرين"), 2, ::inflection::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, u"١١ تذكيرا", u"إحدى عشر تذكيرا", 11, u"تذكير");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"١١ تذكيرا", u"إحدى عشر تذكيرا"), 11, ::inflection::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"١١ تذكيرا", u"إحدى عشر تذكيرا"), 11, ::inflection::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, u"١٠٠ تذكير", u"مائة تذكير", 100, u"تذكير");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"١٠٠ تذكير", u"مائة تذكير"), 100, ::inflection::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"١٠٠ تذكير", u"مائة تذكير"), 100, ::inflection::dialog::SpeakableString(u"تذكير"), u"genitive");

    // dose, portion, swallow of a medicine or drinks
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربة"), 1, ::inflection::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربة"), 1, ::inflection::dialog::SpeakableString(u"شربة"), u"accusative", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربة"), 1, ::inflection::dialog::SpeakableString(u"شربة"), u"genitive", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربتا"), 2, ::inflection::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربتي"), 2, ::inflection::dialog::SpeakableString(u"شربة"), u"accusative", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"شربتي"), 2, ::inflection::dialog::SpeakableString(u"شربة"), u"genitive", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"١١ شربة", u"إحدى عشر شربة"), 11, ::inflection::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::inflection::dialog::SpeakableString(u"١٠٠ شربة", u"مائة شربة"), 100, ::inflection::dialog::SpeakableString(u"شربة"), u"", u"construct");
}

static void testIncrementally(const inflection::dialog::CommonConceptFactory* conceptFactory, int64_t value, const inflection::dialog::SemanticFeatureConceptBase* inflectableConcept, const ::std::vector<::inflection::dialog::SpeakableString>& testCases)
{
    for (const auto& r : testCases) {
        inflection::dialog::NumberConcept number(value++, npc(npc(conceptFactory)->getSemanticFeatureModel())->getLocale(), npc(npc(conceptFactory)->getSemanticFeatureModel())->getLocale());
        if (r.isEmpty()) {
            continue;
        }
        auto quantityConcept = npc(conceptFactory)->quantify(number, inflectableConcept);
        auto quantifiedValue = npc(quantityConcept)->toSpeakableString();
        //INFO(::std::string("expected=") + ::inflection::util::StringUtils::to_string(r.toString()) + " result=" + ::inflection::util::StringUtils::to_string(npc(quantifiedValue)->toString()));
        if (r.speakEqualsPrint()) {
            CHECK(r.getPrint() == npc(quantifiedValue)->getPrint());
        } else {
            CHECK(r == *npc(quantifiedValue));
        }
        delete quantityConcept;
        delete quantifiedValue;
    }
}

static void testIncrementally(const inflection::dialog::CommonConceptFactory* conceptFactory, int32_t value, const std::u16string& conceptStr, const ::std::vector<::inflection::dialog::SpeakableString>& testCases)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, conceptStr)));
    testIncrementally(conceptFactory, value, inflectableConcept.get(), testCases);
}

static void testIncrementally(const inflection::dialog::CommonConceptFactory* conceptFactory, int32_t value, inflection::dialog::SemanticFeatureConceptBase* inflectableConcept, const ::std::vector<::std::u16string>&  testCases)
{
    ::std::vector<::inflection::dialog::SpeakableString> speakableTestCases;
    speakableTestCases.reserve(testCases.size());
    for (const auto& testCase : testCases) {
        speakableTestCases.emplace_back(testCase);
    }
    testIncrementally(conceptFactory, value, inflectableConcept, speakableTestCases);
}

static void testIncrementally(const inflection::dialog::CommonConceptFactory* conceptFactory, int32_t value, const std::u16string& conceptStr, const ::std::vector<::std::u16string>& testCases)
{
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, conceptStr)));
    testIncrementally(conceptFactory, value, inflectableConcept.get(), testCases);
}

TEST_CASE("QuantifyTest#testHebrew")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::ISRAEL());
    auto const featureModel = npc(conceptFactory)->getSemanticFeatureModel();
    auto const definitenessFeature = npc(featureModel)->getFeature(u"definiteness");
    auto const carConcept = toInflectableStringConcept(conceptFactory, u"מכונית");
    testIncrementally(conceptFactory, 0, carConcept, {
        u"0 מכוניות"
        , u"מכונית אחת"
        , u"שתי מכוניות"
        , u"3 מכוניות"
    });
    npc(carConcept)->putConstraint(*npc(definitenessFeature), u"definite");
    testIncrementally(conceptFactory, 0, carConcept, {
        u"0 המכוניות"
        , u"המכונית היחידה"
        , u"שתי המכוניות"
        , u"3 המכוניות"
    });
    auto const bookConcept = toInflectableStringConcept(conceptFactory, u"ספר");
    testIncrementally(conceptFactory, 0, bookConcept, {
        u"0 ספרים"
        , u"ספר אחד"
        , u"שני ספרים"
        , u"3 ספרים"
    });
    npc(bookConcept)->putConstraint(*npc(definitenessFeature), u"definite");
    testIncrementally(conceptFactory, 0, bookConcept, {
        u"0 הספרים"
        , u"הספר היחיד"
        , u"שני הספרים"
        , u"3 הספרים"
    });
    auto const girlConcept = toInflectableStringConcept(conceptFactory, u"בת");
    testIncrementally(conceptFactory, 0, girlConcept, {
            u"0 בנות"
            , u"בת אחת"
            , u"שתי בנות"
            , u"3 בנות"
    });
    npc(girlConcept)->putConstraint(*npc(definitenessFeature), u"definite");
    testIncrementally(conceptFactory, 0, girlConcept, {
            u"0 הבנות"
            , u"הבת היחידה"
            , u"שתי הבנות"
            , u"3 הבנות"
    });
    testIncrementally(conceptFactory, 0, u"חודש", ::std::vector<::std::u16string>({
        u"0 חודשים"
        , u"חודש"
    }));

    testIncrementally(conceptFactory, 3, u"חודש", ::std::vector<::std::u16string>({u"3 חודשים"}));
    testIncrementally(conceptFactory, 11, u"מטר", {::inflection::dialog::SpeakableString(u"11 מטרים", u"אחד עשר מטרים")});
    testIncrementally(conceptFactory, 50, u"מטר", {::inflection::dialog::SpeakableString(u"50 מטרים", u"חמישים מטרים")});
    auto const catConcept = toInflectableStringConcept(conceptFactory, u"חתול");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"0 חתולים", u"אפס חתולים"), 0, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"חתול אחד", u"חתול אחד"), 1, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"שני חתולים", u"שני חתולים"), 2, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"3 חתולים", u"שלושה חתולים"), 3, catConcept);
    npc(catConcept)->putConstraint(*npc(definitenessFeature), u"definite");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"0 החתולים", u"אפס החתולים"), 0, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"החתול היחיד", u"החתול היחיד"), 1, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"שני החתולים", u"שני החתולים"), 2, catConcept);
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"3 החתולים", u"שלושת החתולים"), 3, catConcept);

    auto const secondConcept = toInflectableStringConcept(conceptFactory, u"שניה");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"30 שניות", u"שלושים שניות"), 30, secondConcept);

    delete carConcept;
    delete bookConcept;
    delete girlConcept;
    delete catConcept;
    delete secondConcept;
}

TEST_CASE("QuantifyTest#testItalian")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::ITALIAN());
    assertQuantity(conceptFactory, u"0 settimane", u"zero settimane", 0, u"settimana");
    assertQuantity(conceptFactory, u"una settimana", u"una settimana", 1, u"settimana");
    assertQuantity(conceptFactory, u"2 settimane", u"due settimane", 2, u"settimana");
    assertQuantity(conceptFactory, u"3 settimane", u"tre settimane", 3, u"settimana");
    assertQuantity(conceptFactory, u"0 metri", u"zero metri", 0, u"metro");
    assertQuantity(conceptFactory, u"un metro", u"un metro", 1, u"metro");
    assertQuantity(conceptFactory, u"2 metri", u"due metri", 2, u"metro");
    assertQuantity(conceptFactory, u"3 metri", u"tre metri", 3, u"metro");
    assertQuantity(conceptFactory, u"0 ore", u"zero ore", 0, u"ora");
    assertQuantity(conceptFactory, u"un’ora", u"un’ora", 1, u"ora");
    assertQuantity(conceptFactory, u"2 ore", u"due ore", 2, u"ora");
    assertQuantity(conceptFactory, u"3 ore", u"tre ore", 3, u"ora");
    assertQuantity(conceptFactory, u"0 yen", u"zero yen", 0, u"yen");
    assertQuantity(conceptFactory, u"uno yen", u"uno yen", 1, u"yen");
    assertQuantity(conceptFactory, u"2 yen", u"due yen", 2, u"yen");
    assertQuantity(conceptFactory, u"3 yen", u"tre yen", 3, u"yen");
}

TEST_CASE("QuantifyTest#testSpanish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::SPANISH());
    assertQuantity(conceptFactory, u"1 nota", u"una nota", 1, u"nota");
    assertQuantity(conceptFactory, u"2 notas", u"dos notas", 2, u"nota");
    assertQuantity(conceptFactory, u"1 recordatorio", u"un recordatorio", 1, u"recordatorio");
    assertQuantity(conceptFactory, u"2 recordatorios", u"dos recordatorios", 2, u"recordatorio");
    assertQuantity(conceptFactory, u"1 paraguas", u"un paraguas", 1, u"paraguas");
    assertQuantity(conceptFactory, u"2 paraguas", u"dos paraguas", 2, u"paraguas");
}

TEST_CASE("QuantifyTest#testFrench")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::FRENCH());
    assertQuantity(conceptFactory, u"1 homme", u"un homme", 1, u"homme");
    assertQuantity(conceptFactory, u"2 hommes", u"deux hommes", 2, u"homme");
    assertQuantity(conceptFactory, u"1 femme", u"une femme", 1, u"femme");
    assertQuantity(conceptFactory, u"2 femmes", u"deux femmes", 2, u"femme");
}

TEST_CASE("QuantifyTest#testPortuguese")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::PORTUGUESE());
    assertQuantity(conceptFactory, u"1 homem", u"um homem", 1, u"homem");
    assertQuantity(conceptFactory, u"2 homens", u"dois homens", 2, u"homem");
    assertQuantity(conceptFactory, u"1 mulheraça", u"uma mulheraça", 1, u"mulheraça");
    assertQuantity(conceptFactory, u"2 mulheraças", u"duas mulheraças", 2, u"mulheraça");
}

TEST_CASE("QuantifyTest#testDanish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::DANISH());
    assertQuantity(conceptFactory, u"1 kvinde", u"en kvinde", 1, u"kvinde");
    assertQuantity(conceptFactory, u"2 kvinder", u"to kvinder", 2, u"kvinde");
    assertQuantity(conceptFactory, u"1 lemma", u"et lemma", 1, u"lemma");
    assertQuantity(conceptFactory, u"2 lemmaer", u"to lemmaer", 2, u"lemma");
}

TEST_CASE("QuantifyTest#testSwedish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::SWEDISH());
    assertQuantity(conceptFactory, u"1 kvinna", u"en kvinna", 1, u"kvinna");
    assertQuantity(conceptFactory, u"2 kvinnor", u"två kvinnor", 2, u"kvinna");
    assertQuantity(conceptFactory, u"1 lemma", u"ett lemma", 1, u"lemma");
    assertQuantity(conceptFactory, u"2 lemman", u"två lemman", 2, u"lemma");
}

TEST_CASE("QuantifyTest#testNorwegian")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::NORWEGIAN());
    assertQuantity(conceptFactory, u"1 mann", u"én mann", 1, u"mann");
    assertQuantity(conceptFactory, u"2 menn", u"to menn", 2, u"mann");
    assertQuantity(conceptFactory, u"1 bildøra", u"ei bildøra", 1, u"bildøra");
    assertQuantity(conceptFactory, u"2 bildørene", u"to bildørene", 2, u"bildøra");
    assertQuantity(conceptFactory, u"1 bokmerke", u"ett bokmerke", 1, u"bokmerke");
    assertQuantity(conceptFactory, u"2 bokmerker", u"to bokmerker", 2, u"bokmerke");
    // There are more than one form of the plural, and the default is to not inflect.
    assertQuantity(conceptFactory, u"1 budskap", u"ett budskap", 1, u"budskap");
    assertQuantity(conceptFactory, u"2 budskap", u"to budskap", 2, u"budskap");
}

TEST_CASE("QuantifyTest#testGerman")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::GERMAN());
    assertQuantity(conceptFactory, u"1 Kind", u"ein Kind", 1, u"Kind");
    assertQuantity(conceptFactory, u"2 Kinder", u"zwei Kinder", 2, u"Kind");
    assertQuantity(conceptFactory, u"1 Kindes", u"eines Kindes", 1, u"Kind", u"genitive");
    assertQuantity(conceptFactory, u"2 Kinder", u"zwei Kinder", 2, u"Kind", u"genitive");
    assertQuantity(conceptFactory, u"1 Kind", u"einem Kind", 1, u"Kind", u"dative");
    assertQuantity(conceptFactory, u"2 Kindern", u"zwei Kindern", 2, u"Kind", u"dative");
    assertQuantity(conceptFactory, u"1 Kind", u"ein Kind", 1, u"Kind", u"accusative");
    assertQuantity(conceptFactory, u"2 Kinder", u"zwei Kinder", 2, u"Kind", u"accusative");
    assertQuantity(conceptFactory, u"1 Film", u"ein Film", 1, u"Film");
    assertQuantity(conceptFactory, u"2 Filme", u"zwei Filme", 2, u"Film");
    assertQuantity(conceptFactory, u"1 Filmes", u"eines Filmes", 1, u"Film", u"genitive");
    assertQuantity(conceptFactory, u"2 Filme", u"zwei Filme", 2, u"Film", u"genitive");
    assertQuantity(conceptFactory, u"1 Film", u"einem Film", 1, u"Film", u"dative");
    assertQuantity(conceptFactory, u"2 Filmen", u"zwei Filmen", 2, u"Film", u"dative");
    assertQuantity(conceptFactory, u"1 Film", u"einen Film", 1, u"Film", u"accusative");
    assertQuantity(conceptFactory, u"2 Filme", u"zwei Filme", 2, u"Film", u"accusative");
    assertQuantity(conceptFactory, u"1 Antwort", u"eine Antwort", 1, u"Antwort");
    assertQuantity(conceptFactory, u"2 Antworten", u"zwei Antworten", 2, u"Antwort");
    assertQuantity(conceptFactory, u"1 Antwort", u"einer Antwort", 1, u"Antwort", u"genitive");
    assertQuantity(conceptFactory, u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"genitive");
    assertQuantity(conceptFactory, u"1 Antwort", u"einer Antwort", 1, u"Antwort", u"dative");
    assertQuantity(conceptFactory, u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"dative");
    assertQuantity(conceptFactory, u"1 Antwort", u"eine Antwort", 1, u"Antwort", u"accusative");
    assertQuantity(conceptFactory, u"2 Antworten", u"zwei Antworten", 2, u"Antwort", u"accusative");
}

TEST_CASE("QuantifyTest#testFinnish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::FINNISH());
    assertQuantity(conceptFactory, u"1 viesti", u"yksi viesti", 1, u"viesti");
    assertQuantity(conceptFactory, u"2 viestiä", u"kaksi viestiä", 2, u"viesti");
    assertQuantity(conceptFactory, u"1 viesti", u"yksi viesti", 1, u"viesti", u"nominative");
    assertQuantity(conceptFactory, u"2 viestiä", u"kaksi viestiä", 2, u"viesti", u"nominative");
    assertQuantity(conceptFactory, u"1 viestiä", u"yhtä viestiä", 1, u"viesti", u"partitive");
    assertQuantity(conceptFactory, u"1 viestin", u"yhden viestin", 1, u"viesti", u"genitive");
    assertQuantity(conceptFactory, u"1 viestissä", u"yhdessä viestissä", 1, u"viesti", u"inessive");
    assertQuantity(conceptFactory, u"1 viestistä", u"yhdestä viestistä", 1, u"viesti", u"elative");
    assertQuantity(conceptFactory, u"1 viestiin", u"yhteen viestiin", 1, u"viesti", u"illative");
    assertQuantity(conceptFactory, u"1 viestillä", u"yhdellä viestillä", 1, u"viesti", u"adessive");
    assertQuantity(conceptFactory, u"1 viestiltä", u"yhdeltä viestiltä", 1, u"viesti", u"ablative");
    assertQuantity(conceptFactory, u"1 viestille", u"yhdelle viestille", 1, u"viesti", u"allative");
    assertQuantity(conceptFactory, u"1 viestinä", u"yhtenä viestinä", 1, u"viesti", u"essive");
    assertQuantity(conceptFactory, u"1 viestiksi", u"yhdeksi viestiksi", 1, u"viesti", u"translative");

    assertQuantity(conceptFactory, u"2 viestiä", u"kahta viestiä", 2, u"viesti", u"partitive");
    assertQuantity(conceptFactory, u"2 viestin", u"kahden viestin", 2, u"viesti", u"genitive");
    assertQuantity(conceptFactory, u"2 viestissä", u"kahdessa viestissä", 2, u"viesti", u"inessive");
    assertQuantity(conceptFactory, u"2 viestistä", u"kahdesta viestistä", 2, u"viesti", u"elative");
    assertQuantity(conceptFactory, u"2 viestiin", u"kahteen viestiin", 2, u"viesti", u"illative");
    assertQuantity(conceptFactory, u"2 viestillä", u"kahdella viestillä", 2, u"viesti", u"adessive");
    assertQuantity(conceptFactory, u"2 viestiltä", u"kahdelta viestiltä", 2, u"viesti", u"ablative");
    assertQuantity(conceptFactory, u"2 viestille", u"kahdelle viestille", 2, u"viesti", u"allative");
    assertQuantity(conceptFactory, u"2 viestinä", u"kahtena viestinä", 2, u"viesti", u"essive");
    assertQuantity(conceptFactory, u"2 viestiksi", u"kahdeksi viestiksi", 2, u"viesti", u"translative");

    // Plurale tantum
    assertQuantity(conceptFactory, u"1 häät", u"yhdet häät", 1, u"häät");
    assertQuantity(conceptFactory, u"2 häät", u"kahdet häät", 2, u"häät");
    assertQuantity(conceptFactory, u"1 häissä", u"yksissä häissä", 1, u"häät", u"inessive");
    assertQuantity(conceptFactory, u"2 häissä", u"kaksissa häissä", 2, u"häät", u"inessive");
    assertQuantity(conceptFactory, u"1 häiden", u"yksien häiden", 1, u"häät", u"genitive");
    assertQuantity(conceptFactory, u"2 häiden", u"kaksien häiden", 2, u"häät", u"genitive");

    // Explicit request for plural
    assertQuantity(conceptFactory, u"1 kisan", u"yhden kisan", 1, u"kisa", u"genitive");
    assertQuantity(conceptFactory, u"2 kisan", u"kahden kisan", 2, u"kisa", u"genitive");
    assertQuantityCount(conceptFactory, ::inflection::dialog::SpeakableString(u"1 kisojen", u"yksien kisojen"), 1, ::inflection::dialog::SpeakableString(u"kisa"), u"genitive", u"plural");
    assertQuantityCount(conceptFactory, ::inflection::dialog::SpeakableString(u"2 kisojen", u"kaksien kisojen"), 2, ::inflection::dialog::SpeakableString(u"kisa"), u"genitive", u"plural");
}

TEST_CASE("QuantifyTest#testRussian")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::RUSSIAN());
    assertQuantity(conceptFactory, u"1 карандаш", u"один карандаш", 1, u"карандаш");
    assertQuantity(conceptFactory, u"2 карандаша", u"два карандаша", 2, u"карандаш");
    assertQuantity(conceptFactory, u"5 карандашей", u"пять карандашей", 5, u"карандаш");
    assertQuantity(conceptFactory, u"1 женщина", u"одна женщина", 1, u"женщина");
    assertQuantity(conceptFactory, u"2 женщины", u"две женщины", 2, u"женщина");
    assertQuantity(conceptFactory, u"5 женщин", u"пять женщин", 5, u"женщина");
    assertQuantity(conceptFactory, u"1 сообщение", u"одно сообщение", 1, u"сообщение");
    assertQuantity(conceptFactory, u"2 сообщения", u"два сообщения", 2, u"сообщение");
    assertQuantity(conceptFactory, u"5 сообщений", u"пять сообщений", 5, u"сообщение");
    assertQuantity(conceptFactory, u"1 стол", u"один стол", 1, u"стол");
    assertQuantity(conceptFactory, u"2 стола", u"два стола", 2, u"стол");
    assertQuantity(conceptFactory, u"5 столов", u"пять столов", 5, u"стол");
    assertQuantity(conceptFactory, u"1 стола", u"одного стола", 1, u"стол", u"genitive");
    assertQuantity(conceptFactory, u"2 столов", u"двух столов", 2, u"стол", u"genitive");
    assertQuantity(conceptFactory, u"5 столов", u"пяти столов", 5, u"стол", u"genitive");
    assertQuantity(conceptFactory, u"1 стол", u"один стол", 1, u"стол", u"accusative");
    assertQuantity(conceptFactory, u"2 стола", u"два стола", 2, u"стол", u"accusative");
    assertQuantity(conceptFactory, u"5 столов", u"пять столов", 5, u"стол", u"accusative");
    assertQuantity(conceptFactory, u"1 столу", u"одному столу", 1, u"стол", u"dative");
    assertQuantity(conceptFactory, u"2 столам", u"двум столам", 2, u"стол", u"dative");
    assertQuantity(conceptFactory, u"5 столам", u"пяти столам", 5, u"стол", u"dative");
    assertQuantity(conceptFactory, u"1 столом", u"одним столом", 1, u"стол", u"instrumental");
    assertQuantity(conceptFactory, u"2 столами", u"двумя столами", 2, u"стол", u"instrumental");
    assertQuantity(conceptFactory, u"5 столами", u"пятью столами", 5, u"стол", u"instrumental");
    assertQuantity(conceptFactory, u"1 столе", u"одном столе", 1, u"стол", u"prepositional");
    assertQuantity(conceptFactory, u"2 столах", u"двух столах", 2, u"стол", u"prepositional");
    assertQuantity(conceptFactory, u"5 столах", u"пяти столах", 5, u"стол", u"prepositional");
    assertQuantity(conceptFactory, u"1,25 сантиметра", u"одна целая двадцать пять сотых сантиметра", 1.25, u"сантиметр");
    assertQuantity(conceptFactory, u"2,25 сантиметра", u"две целых двадцать пять сотых сантиметра", 2.25, u"сантиметр");
    assertQuantity(conceptFactory, u"5,25 сантиметра", u"пять целых двадцать пять сотых сантиметра", 5.25, u"сантиметр");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1,25 QQQ"), 1.25, ::inflection::dialog::SpeakableString(u"QQQ")); // nonsense
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2 QQQ"), 2, ::inflection::dialog::SpeakableString(u"QQQ")); // nonsense

    // Deprecated case names that no longer need support.
    assertQuantity(conceptFactory, u"1 столом", u"одним столом", 1, u"стол", u"ablative");
    assertQuantity(conceptFactory, u"2 столами", u"двумя столами", 2, u"стол", u"ablative");
    assertQuantity(conceptFactory, u"5 столами", u"пятью столами", 5, u"стол", u"ablative");
    assertQuantity(conceptFactory, u"1 столе", u"одном столе", 1, u"стол", u"locative");
    assertQuantity(conceptFactory, u"2 столах", u"двух столах", 2, u"стол", u"locative");
    assertQuantity(conceptFactory, u"5 столах", u"пяти столах", 5, u"стол", u"locative");
}

TEST_CASE("QuantifyTest#testPolish")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::POLISH());
    assertQuantity(conceptFactory, u"1 lekarz", u"jeden lekarz", 1, u"lekarz"); // masculine personal
    assertQuantity(conceptFactory, u"2 lekarze", u"dwaj lekarze", 2, u"lekarz");
    assertQuantity(conceptFactory, u"5 lekarzy", u"pięciu lekarzy", 5, u"lekarz");
    assertQuantity(conceptFactory, u"1 kilometr", u"jeden kilometr", 1, u"kilometr"); // masculine inanimate
    assertQuantity(conceptFactory, u"2 kilometry", u"dwa kilometry", 2, u"kilometr");
    assertQuantity(conceptFactory, u"5 kilometrów", u"pięć kilometrów", 5, u"kilometr");
    assertQuantity(conceptFactory, u"1 lampa", u"jedna lampa", 1, u"lampa"); // feminine
    assertQuantity(conceptFactory, u"2 lampy", u"dwie lampy", 2, u"lampa");
    assertQuantity(conceptFactory, u"5 lamp", u"pięć lamp", 5, u"lampa");
    assertQuantity(conceptFactory, u"1 światło", u"jedno światło", 1, u"światło"); // neuter
    assertQuantity(conceptFactory, u"2 światła", u"dwa światła", 2, u"światło");
    assertQuantity(conceptFactory, u"5 świateł", u"pięć świateł", 5, u"światło");
    assertQuantity(conceptFactory, u"1 lekarza", u"jednego lekarza", 1, u"lekarz", u"genitive");
    assertQuantity(conceptFactory, u"2 lekarzy", u"dwóch lekarzy", 2, u"lekarz", u"genitive");
    assertQuantity(conceptFactory, u"5 lekarzy", u"pięciu lekarzy", 5, u"lekarz", u"genitive");
    assertQuantity(conceptFactory, u"1 lekarza", u"jednego lekarza", 1, u"lekarz", u"accusative"); // masculine personal
    assertQuantity(conceptFactory, u"2 lekarzy", u"dwóch lekarzy", 2, u"lekarz", u"accusative");
    assertQuantity(conceptFactory, u"5 lekarzy", u"pięciu lekarzy", 5, u"lekarz", u"accusative");
    assertQuantity(conceptFactory, u"1 byka", u"jednego byka", 1, u"byk", u"accusative"); // masculine animate
    assertQuantity(conceptFactory, u"2 byki", u"dwa byki", 2, u"byk", u"accusative");
    assertQuantity(conceptFactory, u"5 byków", u"pięć byków", 5, u"byk", u"accusative");
    assertQuantity(conceptFactory, u"1 kilometr", u"jeden kilometr", 1, u"kilometr", u"accusative"); // masculine inanimate
    assertQuantity(conceptFactory, u"2 kilometry", u"dwa kilometry", 2, u"kilometr", u"accusative");
    assertQuantity(conceptFactory, u"5 kilometrów", u"pięć kilometrów", 5, u"kilometr", u"accusative");
    assertQuantity(conceptFactory, u"1 lampę", u"jedną lampę", 1, u"lampa", u"accusative"); // feminine
    assertQuantity(conceptFactory, u"2 lampy", u"dwie lampy", 2, u"lampa", u"accusative");
    assertQuantity(conceptFactory, u"5 lamp", u"pięć lamp", 5, u"lampa", u"accusative");
    assertQuantity(conceptFactory, u"1 światło", u"jedno światło", 1, u"światło", u"accusative"); // neuter
    assertQuantity(conceptFactory, u"2 światła", u"dwa światła", 2, u"światło", u"accusative");
    assertQuantity(conceptFactory, u"5 świateł", u"pięć świateł", 5, u"światło", u"accusative");
    assertQuantity(conceptFactory, u"1 lekarzowi", u"jednemu lekarzowi", 1, u"lekarz", u"dative");
    assertQuantity(conceptFactory, u"2 lekarzom", u"dwóm lekarzom", 2, u"lekarz", u"dative");
    assertQuantity(conceptFactory, u"5 lekarzom", u"pięciu lekarzom", 5, u"lekarz", u"dative");
    assertQuantity(conceptFactory, u"1 lekarzem", u"jednym lekarzem", 1, u"lekarz", u"instrumental");
    assertQuantity(conceptFactory, u"2 lekarzami", u"dwoma lekarzami", 2, u"lekarz", u"instrumental");
    assertQuantity(conceptFactory, u"5 lekarzami", u"pięcioma lekarzami", 5, u"lekarz", u"instrumental");
    assertQuantity(conceptFactory, u"1 lekarzu", u"jednym lekarzu", 1, u"lekarz", u"locative");
    assertQuantity(conceptFactory, u"2 lekarzach", u"dwóch lekarzach", 2, u"lekarz", u"locative");
    assertQuantity(conceptFactory, u"5 lekarzach", u"pięciu lekarzach", 5, u"lekarz", u"locative");
    assertQuantity(conceptFactory, u"1,25 kilometra", u"jeden przecinek dwadzieścia pięć kilometra", 1.25, u"kilometr");
    assertQuantity(conceptFactory, u"2,25 kilometra", u"dwa przecinek dwadzieścia pięć kilometra", 2.25, u"kilometr");
    assertQuantity(conceptFactory, u"5,25 kilometra", u"pięć przecinek dwadzieścia pięć kilometra", 5.25, u"kilometr");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1,25 QQQ"), 1.25, ::inflection::dialog::SpeakableString(u"QQQ")); // nonsense
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2 QQQ"), 2, ::inflection::dialog::SpeakableString(u"QQQ")); // nonsense
}

TEST_CASE("QuantifyTest#testJapanese")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::JAPANESE());
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1word"), 1, ::inflection::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::inflection::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testMandarin")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::CHINA());
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1word"), 1, ::inflection::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::inflection::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testCantonese")
{
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::HONGKONG_CANTONESE());
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1word"), 1, ::inflection::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::inflection::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::inflection::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testMalayalam")
{
    // In Malayalam the number follows the noun when singular (CLDR "one") and
    // precedes the noun for every other count. See MlCommonConceptFactory.
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::MALAYALAM());
    assertQuantity(conceptFactory, u"പുസ്തകം 1", 1, u"പുസ്തകം");
    assertQuantity(conceptFactory, u"2 പുസ്തകങ്ങളെ", 2, u"പുസ്തകം");
}

TEST_CASE("QuantifyTest#testSerboCroatian")
{
    // Serbian (Bosnian-Croatian-Serbian) numeral-noun agreement: 1 -> nominative singular,
    // 2-4 (paucal) -> genitive singular, 5+ -> genitive plural, and any fraction -> genitive
    // singular. Exercise every state with the masculine noun "брод" (ship).
    auto const conceptFactory = getCommonConceptFactory(::inflection::util::LocaleUtils::SERBIAN());
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1 брод", u"један брод"), 1, ::inflection::dialog::SpeakableString(u"брод"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2 брода", u"два брода"), 2, ::inflection::dialog::SpeakableString(u"брод"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"3 брода", u"три брода"), 3, ::inflection::dialog::SpeakableString(u"брод"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"4 брода", u"четири брода"), 4, ::inflection::dialog::SpeakableString(u"брод"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"5 бродова", u"пет бродова"), 5, ::inflection::dialog::SpeakableString(u"брод"));
#if U_ICU_VERSION_MAJOR_NUM <= 78
    // TODO remove this version check after version 79 is released, and update versions.mk with the new minimum version.
    WARN("U_ICU_VERSION_MAJOR_NUM <= 78. Please test with a newer version of ICU.");
#else
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1,1 брода", u"један зарез један брода"), 1.1, ::inflection::dialog::SpeakableString(u"брод"));
#endif

    // Feminine noun "земља" (land) through the same states.
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1 земља", u"једна земља"), 1, ::inflection::dialog::SpeakableString(u"земља"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"2 земље", u"две земље"), 2, ::inflection::dialog::SpeakableString(u"земља"));
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"5 земаља", u"пет земаља"), 5, ::inflection::dialog::SpeakableString(u"земља"));
#if U_ICU_VERSION_MAJOR_NUM <= 78
    // TODO remove this version check after version 79 is released, and update versions.mk with the new minimum version.
    WARN("U_ICU_VERSION_MAJOR_NUM <= 78. Please test with a newer version of ICU.");
#else
    assertQuantity(conceptFactory, ::inflection::dialog::SpeakableString(u"1,1 земље", u"једна зарез једна земље"), 1.1, ::inflection::dialog::SpeakableString(u"земља"));
#endif

    // Verify oblique cases for 5+ (always genitive plural: бродова / земаља)
    assertQuantity(conceptFactory, u"5 бродова", u"пет бродова", 5, u"брод", u"locative");
    assertQuantity(conceptFactory, u"5 бродова", u"пет бродова", 5, u"брод", u"instrumental");
    assertQuantity(conceptFactory, u"5 бродова", u"пет бродова", 5, u"брод", u"dative");
    assertQuantity(conceptFactory, u"5 земаља", u"пет земаља", 5, u"земља", u"locative");
    assertQuantity(conceptFactory, u"5 земаља", u"пет земаља", 5, u"земља", u"instrumental");
}

