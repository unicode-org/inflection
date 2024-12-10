/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/CommonConceptFactory.hpp>
#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>
#include <string>
#include <utility>

const morphuntion::dialog::CommonConceptFactory* getCommonConceptFactory(const ::morphuntion::util::ULocale& language)
{
    return npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(language);
}

morphuntion::dialog::InflectableStringConcept* toInflectableStringConcept(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const ::morphuntion::dialog::SpeakableString& phrase)
{
    return new morphuntion::dialog::InflectableStringConcept(npc(conceptFactory)->getSemanticFeatureModel(), phrase);
}

morphuntion::dialog::InflectableStringConcept* toInflectableStringConcept(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const std::u16string& phrase)
{
    return new morphuntion::dialog::InflectableStringConcept(npc(conceptFactory)->getSemanticFeatureModel(), ::morphuntion::dialog::SpeakableString(phrase));
}

template<typename T>
static void assertQuantity(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const ::morphuntion::dialog::SpeakableString& expected, T count, const morphuntion::dialog::SemanticFeatureConceptBase* fromPhrase)
{
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> value(npc(std::is_same_v<T, double>
            ? npc(conceptFactory)->number(npc(conceptFactory)->getLanguage(), (double)count)
            : npc(conceptFactory)->number(npc(conceptFactory)->getLanguage(), (int64_t)count)));
    ::std::unique_ptr<::morphuntion::dialog::SpeakableString> result(npc(npc(conceptFactory)->quantify(*value, fromPhrase)));
    INFO(::std::string("expected=") + ::morphuntion::util::StringUtils::to_string(expected.toString()) + " result=" + ::morphuntion::util::StringUtils::to_string(result->toString()));
    CHECK(expected == *result);
}

template<typename T>
static void assertQuantity(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const ::morphuntion::dialog::SpeakableString& expected, T count, const ::morphuntion::dialog::SpeakableString& from)
{
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, from)));
    assertQuantity(conceptFactory, expected, count, fromPhrase.get());
}

template<typename T>
static void assertQuantity(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const std::u16string& expected, T count, const ::std::u16string& from)
{
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(expected), count, ::morphuntion::dialog::SpeakableString(from));
}

static void assertQuantity(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const ::morphuntion::dialog::SpeakableString& expected, int64_t count, const ::morphuntion::dialog::SpeakableString& from, const std::u16string& caseString)
{
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, from)));
    auto caseFeature = npc(npc(conceptFactory)->getSemanticFeatureModel())->getFeature(u"case");
    inflectableConcept->putConstraint(*npc(caseFeature), caseString);
    assertQuantity(conceptFactory, expected, count, inflectableConcept.get());
}

static void assertQuantityDefiniteness(const morphuntion::dialog::CommonConceptFactory* conceptFactory, const ::morphuntion::dialog::SpeakableString& expected, int64_t count, const ::morphuntion::dialog::SpeakableString& from, const std::u16string& caseString, const std::u16string& definitenessString)
{
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, from)));
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
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::US());
    assertQuantity(conceptFactory, u"1 yen", 1, u"yen");
    assertQuantity(conceptFactory, u"2 yen", 2, u"yen");
    assertQuantity(conceptFactory, u"1 kilometer", 1, u"kilometer");
    assertQuantity(conceptFactory, u"2 kilometers", 2, u"kilometer");
}

TEST_CASE("QuantifyTest#testThai")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::THAILAND());
    assertQuantity(conceptFactory, u"restaurant 1", 1, u"restaurant");
    assertQuantity(conceptFactory, u"restaurant 2", 2, u"restaurant");
}

TEST_CASE("QuantifyTest#testKorean")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::KOREA());
    assertQuantity(conceptFactory, u"1 restaurant", 1, u"restaurant");
    assertQuantity(conceptFactory, u"2 restaurant", 2, u"restaurant");
    assertQuantity(conceptFactory, u"3개", 3, u"개");
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::morphuntion::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Word Blah"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testArabic")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::SAUDI_ARABIA());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"٠ رسالة", u"صفر رسالة"), 0, ::morphuntion::dialog::SpeakableString(u"رسالة"));
    assertQuantity(conceptFactory, u"رسالة", 1, u"رسالة");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"رسالة"), 1, ::morphuntion::dialog::SpeakableString(u"رسالة"), u"accusative");
    assertQuantity(conceptFactory, u"رسالتان", 2, u"رسالة");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"رسالتين"), 2, ::morphuntion::dialog::SpeakableString(u"رسالة"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"رسالتين"), 2, ::morphuntion::dialog::SpeakableString(u"رسالة"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"٣ رسائل", u"ثلاثة رسائل"), 3, ::morphuntion::dialog::SpeakableString(u"رسالة"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١١ رسالة", u"إحدى عشر رسالة"), 11, ::morphuntion::dialog::SpeakableString(u"رسالة"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١٠٠ رسالة", u"مائة رسالة"), 100, ::morphuntion::dialog::SpeakableString(u"رسالة"));

    // reminder
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكير"), 1, ::morphuntion::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكيرا"), 1, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكير"), 1, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكيران"), 2, ::morphuntion::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكيرين"), 2, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"تذكيرين"), 2, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١١ تذكيرا", u"إحدى عشر تذكيرا"), 11, ::morphuntion::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١١ تذكيرا", u"إحدى عشر تذكيرا"), 11, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١١ تذكيرا", u"إحدى عشر تذكيرا"), 11, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١٠٠ تذكير", u"مائة تذكير"), 100, ::morphuntion::dialog::SpeakableString(u"تذكير"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١٠٠ تذكير", u"مائة تذكير"), 100, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١٠٠ تذكير", u"مائة تذكير"), 100, ::morphuntion::dialog::SpeakableString(u"تذكير"), u"genitive");

    // dose, portion, swallow of a medicine or drinks
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربة"), 1, ::morphuntion::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربة"), 1, ::morphuntion::dialog::SpeakableString(u"شربة"), u"accusative", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربة"), 1, ::morphuntion::dialog::SpeakableString(u"شربة"), u"genitive", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربتا"), 2, ::morphuntion::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربتي"), 2, ::morphuntion::dialog::SpeakableString(u"شربة"), u"accusative", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"شربتي"), 2, ::morphuntion::dialog::SpeakableString(u"شربة"), u"genitive", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١١ شربة", u"إحدى عشر شربة"), 11, ::morphuntion::dialog::SpeakableString(u"شربة"), u"", u"construct");
    assertQuantityDefiniteness(conceptFactory, ::morphuntion::dialog::SpeakableString(u"١٠٠ شربة", u"مائة شربة"), 100, ::morphuntion::dialog::SpeakableString(u"شربة"), u"", u"construct");
}

static void testIncrementally(const morphuntion::dialog::CommonConceptFactory* conceptFactory, int64_t value, const morphuntion::dialog::SemanticFeatureConceptBase* inflectableConcept, const ::std::vector<::morphuntion::dialog::SpeakableString>& testCases)
{
    for (const auto& r : testCases) {
        morphuntion::dialog::NumberConcept number(value++, npc(npc(conceptFactory)->getSemanticFeatureModel())->getLocale(), npc(npc(conceptFactory)->getSemanticFeatureModel())->getLocale());
        if (r.isEmpty()) {
            continue;
        }
        auto quantityConcept = npc(conceptFactory)->quantify(number, inflectableConcept);
        auto quantifiedValue = npc(quantityConcept)->toSpeakableString();
        //INFO(::std::string("expected=") + ::morphuntion::util::StringUtils::to_string(r.toString()) + " result=" + ::morphuntion::util::StringUtils::to_string(npc(quantifiedValue)->toString()));
        if (r.speakEqualsPrint()) {
            CHECK(r.getPrint() == npc(quantifiedValue)->getPrint());
        } else {
            CHECK(r == *npc(quantifiedValue));
        }
        delete quantityConcept;
        delete quantifiedValue;
    }
}

static void testIncrementally(const morphuntion::dialog::CommonConceptFactory* conceptFactory, int32_t value, const std::u16string& conceptStr, const ::std::vector<::morphuntion::dialog::SpeakableString>& testCases)
{
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, conceptStr)));
    testIncrementally(conceptFactory, value, inflectableConcept.get(), testCases);
}

static void testIncrementally(const morphuntion::dialog::CommonConceptFactory* conceptFactory, int32_t value, morphuntion::dialog::SemanticFeatureConceptBase* inflectableConcept, const ::std::vector<::std::u16string>&  testCases)
{
    ::std::vector<::morphuntion::dialog::SpeakableString> speakableTestCases;
    speakableTestCases.reserve(testCases.size());
    for (const auto& testCase : testCases) {
        speakableTestCases.emplace_back(testCase);
    }
    testIncrementally(conceptFactory, value, inflectableConcept, speakableTestCases);
}

static void testIncrementally(const morphuntion::dialog::CommonConceptFactory* conceptFactory, int32_t value, const std::u16string& conceptStr, const ::std::vector<::std::u16string>& testCases)
{
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> inflectableConcept(npc(toInflectableStringConcept(conceptFactory, conceptStr)));
    testIncrementally(conceptFactory, value, inflectableConcept.get(), testCases);
}

TEST_CASE("QuantifyTest#testHebrew")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::ISRAEL());
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
    testIncrementally(conceptFactory, 11, u"מטר", {::morphuntion::dialog::SpeakableString(u"11 מטרים", u"אחד עשר מטרים")});
    testIncrementally(conceptFactory, 50, u"מטר", {::morphuntion::dialog::SpeakableString(u"50 מטרים", u"חמישים מטרים")});
    auto const catConcept = toInflectableStringConcept(conceptFactory, u"חתול");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 חתולים", u"אפס חתולים"), 0, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"חתול אחד", u"חתול אחד"), 1, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"שני חתולים", u"שני חתולים"), 2, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 חתולים", u"שלושה חתולים"), 3, catConcept);
    npc(catConcept)->putConstraint(*npc(definitenessFeature), u"definite");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 החתולים", u"אפס החתולים"), 0, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"החתול היחיד", u"החתול היחיד"), 1, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"שני החתולים", u"שני החתולים"), 2, catConcept);
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 החתולים", u"שלושת החתולים"), 3, catConcept);

    auto const secondConcept = toInflectableStringConcept(conceptFactory, u"שניה");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"30 שניות", u"שלושים שניות"), 30, secondConcept);

    delete carConcept;
    delete bookConcept;
    delete girlConcept;
    delete catConcept;
    delete secondConcept;
}

TEST_CASE("QuantifyTest#testItalian")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::ITALIAN());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 settimane", u"zero settimane"), 0, ::morphuntion::dialog::SpeakableString(u"settimana"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"una settimana", u"una settimana"), 1, ::morphuntion::dialog::SpeakableString(u"settimana"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 settimane", u"due settimane"), 2, ::morphuntion::dialog::SpeakableString(u"settimana"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 settimane", u"tre settimane"), 3, ::morphuntion::dialog::SpeakableString(u"settimana"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 metri", u"zero metri"), 0, ::morphuntion::dialog::SpeakableString(u"metro"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"un metro", u"un metro"), 1, ::morphuntion::dialog::SpeakableString(u"metro"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 metri", u"due metri"), 2, ::morphuntion::dialog::SpeakableString(u"metro"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 metri", u"tre metri"), 3, ::morphuntion::dialog::SpeakableString(u"metro"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 ore", u"zero ore"), 0, ::morphuntion::dialog::SpeakableString(u"ora"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"un’ora", u"un’ora"), 1, ::morphuntion::dialog::SpeakableString(u"ora"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 ore", u"due ore"), 2, ::morphuntion::dialog::SpeakableString(u"ora"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 ore", u"tre ore"), 3, ::morphuntion::dialog::SpeakableString(u"ora"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"0 yen", u"zero yen"), 0, ::morphuntion::dialog::SpeakableString(u"yen"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"uno yen", u"uno yen"), 1, ::morphuntion::dialog::SpeakableString(u"yen"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 yen", u"due yen"), 2, ::morphuntion::dialog::SpeakableString(u"yen"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"3 yen", u"tre yen"), 3, ::morphuntion::dialog::SpeakableString(u"yen"));
}

TEST_CASE("QuantifyTest#testSpanish")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::SPANISH());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 nota", u"una nota"), 1, ::morphuntion::dialog::SpeakableString(u"nota"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 notas", u"dos notas"), 2, ::morphuntion::dialog::SpeakableString(u"nota"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 recordatorio", u"un recordatorio"), 1, ::morphuntion::dialog::SpeakableString(u"recordatorio"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 recordatorios", u"dos recordatorios"), 2, ::morphuntion::dialog::SpeakableString(u"recordatorio"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 paraguas", u"un paraguas"), 1, ::morphuntion::dialog::SpeakableString(u"paraguas"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 paraguas", u"dos paraguas"), 2, ::morphuntion::dialog::SpeakableString(u"paraguas"));
}

TEST_CASE("QuantifyTest#testFrench")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::FRENCH());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 homme", u"un homme"), 1, ::morphuntion::dialog::SpeakableString(u"homme"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 hommes", u"deux hommes"), 2, ::morphuntion::dialog::SpeakableString(u"homme"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 femme", u"une femme"), 1, ::morphuntion::dialog::SpeakableString(u"femme"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 femmes", u"deux femmes"), 2, ::morphuntion::dialog::SpeakableString(u"femme"));
}

TEST_CASE("QuantifyTest#testPortuguese")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::PORTUGUESE());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 homem", u"um homem"), 1, ::morphuntion::dialog::SpeakableString(u"homem"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 homens", u"dois homens"), 2, ::morphuntion::dialog::SpeakableString(u"homem"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 mulheraça", u"uma mulheraça"), 1, ::morphuntion::dialog::SpeakableString(u"mulheraça"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 mulheraças", u"duas mulheraças"), 2, ::morphuntion::dialog::SpeakableString(u"mulheraça"));
}

TEST_CASE("QuantifyTest#testDanish")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::DANISH());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 kvinde", u"en kvinde"), 1, ::morphuntion::dialog::SpeakableString(u"kvinde"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 kvinder", u"to kvinder"), 2, ::morphuntion::dialog::SpeakableString(u"kvinde"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 lemma", u"et lemma"), 1, ::morphuntion::dialog::SpeakableString(u"lemma"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 lemmaer", u"to lemmaer"), 2, ::morphuntion::dialog::SpeakableString(u"lemma"));
}

TEST_CASE("QuantifyTest#testSwedish")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::SWEDISH());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 kvinna", u"en kvinna"), 1, ::morphuntion::dialog::SpeakableString(u"kvinna"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 kvinnor", u"två kvinnor"), 2, ::morphuntion::dialog::SpeakableString(u"kvinna"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 lemma", u"ett lemma"), 1, ::morphuntion::dialog::SpeakableString(u"lemma"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 lemman", u"två lemman"), 2, ::morphuntion::dialog::SpeakableString(u"lemma"));
}

TEST_CASE("QuantifyTest#testNorwegian")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::NORWEGIAN());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 mann", u"én mann"), 1, ::morphuntion::dialog::SpeakableString(u"mann"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 menn", u"to menn"), 2, ::morphuntion::dialog::SpeakableString(u"mann"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 bildøra", u"ei bildøra"), 1, ::morphuntion::dialog::SpeakableString(u"bildøra"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 bildørene", u"to bildørene"), 2, ::morphuntion::dialog::SpeakableString(u"bildøra"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 bokmerke", u"ett bokmerke"), 1, ::morphuntion::dialog::SpeakableString(u"bokmerke"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 bokmerker", u"to bokmerker"), 2, ::morphuntion::dialog::SpeakableString(u"bokmerke"));
    // There are more than one form of the plural, and the default is to not inflect.
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 budskap", u"ett budskap"), 1, ::morphuntion::dialog::SpeakableString(u"budskap"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 budskap", u"to budskap"), 2, ::morphuntion::dialog::SpeakableString(u"budskap"));
}

TEST_CASE("QuantifyTest#testGerman")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::GERMAN());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Kind", u"ein Kind"), 1, ::morphuntion::dialog::SpeakableString(u"Kind"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Kinder", u"zwei Kinder"), 2, ::morphuntion::dialog::SpeakableString(u"Kind"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Kindes", u"eines Kindes"), 1, ::morphuntion::dialog::SpeakableString(u"Kind"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Kinder", u"zwei Kinder"), 2, ::morphuntion::dialog::SpeakableString(u"Kind"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Kind", u"einem Kind"), 1, ::morphuntion::dialog::SpeakableString(u"Kind"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Kindern", u"zwei Kindern"), 2, ::morphuntion::dialog::SpeakableString(u"Kind"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Kind", u"ein Kind"), 1, ::morphuntion::dialog::SpeakableString(u"Kind"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Kinder", u"zwei Kinder"), 2, ::morphuntion::dialog::SpeakableString(u"Kind"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Film", u"ein Film"), 1, ::morphuntion::dialog::SpeakableString(u"Film"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Filme", u"zwei Filme"), 2, ::morphuntion::dialog::SpeakableString(u"Film"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Films", u"eines Films"), 1, ::morphuntion::dialog::SpeakableString(u"Film"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Filme", u"zwei Filme"), 2, ::morphuntion::dialog::SpeakableString(u"Film"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Film", u"einem Film"), 1, ::morphuntion::dialog::SpeakableString(u"Film"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Filmen", u"zwei Filmen"), 2, ::morphuntion::dialog::SpeakableString(u"Film"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Film", u"einen Film"), 1, ::morphuntion::dialog::SpeakableString(u"Film"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Filme", u"zwei Filme"), 2, ::morphuntion::dialog::SpeakableString(u"Film"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Antwort", u"eine Antwort"), 1, ::morphuntion::dialog::SpeakableString(u"Antwort"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Antworten", u"zwei Antworten"), 2, ::morphuntion::dialog::SpeakableString(u"Antwort"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Antwort", u"einer Antwort"), 1, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Antworten", u"zwei Antworten"), 2, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Antwort", u"einer Antwort"), 1, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Antworten", u"zwei Antworten"), 2, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 Antwort", u"eine Antwort"), 1, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 Antworten", u"zwei Antworten"), 2, ::morphuntion::dialog::SpeakableString(u"Antwort"), u"accusative");
}

TEST_CASE("QuantifyTest#testRussian")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::RUSSIAN());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 карандаш", u"один карандаш"), 1, ::morphuntion::dialog::SpeakableString(u"карандаш"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 карандаша", u"два карандаша"), 2, ::morphuntion::dialog::SpeakableString(u"карандаш"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 карандашей", u"пять карандашей"), 5, ::morphuntion::dialog::SpeakableString(u"карандаш"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 женщина", u"одна женщина"), 1, ::morphuntion::dialog::SpeakableString(u"женщина"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 женщины", u"две женщины"), 2, ::morphuntion::dialog::SpeakableString(u"женщина"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 женщин", u"пять женщин"), 5, ::morphuntion::dialog::SpeakableString(u"женщина"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 сообщение", u"одно сообщение"), 1, ::morphuntion::dialog::SpeakableString(u"сообщение"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 сообщения", u"два сообщения"), 2, ::morphuntion::dialog::SpeakableString(u"сообщение"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 сообщений", u"пять сообщений"), 5, ::morphuntion::dialog::SpeakableString(u"сообщение"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 стол", u"один стол"), 1, ::morphuntion::dialog::SpeakableString(u"стол"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 стола", u"два стола"), 2, ::morphuntion::dialog::SpeakableString(u"стол"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столов", u"пять столов"), 5, ::morphuntion::dialog::SpeakableString(u"стол"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 стола", u"одного стола"), 1, ::morphuntion::dialog::SpeakableString(u"стол"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 столов", u"двух столов"), 2, ::morphuntion::dialog::SpeakableString(u"стол"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столов", u"пяти столов"), 5, ::morphuntion::dialog::SpeakableString(u"стол"), u"genitive");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 стол", u"один стол"), 1, ::morphuntion::dialog::SpeakableString(u"стол"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 стола", u"два стола"), 2, ::morphuntion::dialog::SpeakableString(u"стол"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столов", u"пять столов"), 5, ::morphuntion::dialog::SpeakableString(u"стол"), u"accusative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 столу", u"одному столу"), 1, ::morphuntion::dialog::SpeakableString(u"стол"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 столам", u"двум столам"), 2, ::morphuntion::dialog::SpeakableString(u"стол"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столам", u"пяти столам"), 5, ::morphuntion::dialog::SpeakableString(u"стол"), u"dative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 столом", u"одним столом"), 1, ::morphuntion::dialog::SpeakableString(u"стол"), u"ablative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 столами", u"двумя столами"), 2, ::morphuntion::dialog::SpeakableString(u"стол"), u"ablative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столами", u"пятью столами"), 5, ::morphuntion::dialog::SpeakableString(u"стол"), u"ablative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1 столе", u"одном столе"), 1, ::morphuntion::dialog::SpeakableString(u"стол"), u"locative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 столах", u"двух столах"), 2, ::morphuntion::dialog::SpeakableString(u"стол"), u"locative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5 столах", u"пяти столах"), 5, ::morphuntion::dialog::SpeakableString(u"стол"), u"locative");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1,25 сантиметра", u"одна целый двадцать пять сотых сантиметра"), 1.25, ::morphuntion::dialog::SpeakableString(u"сантиметр"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2,25 сантиметра", u"две целых двадцать пять сотых сантиметра"), 2.25, ::morphuntion::dialog::SpeakableString(u"сантиметр"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"5,25 сантиметра", u"пять целых двадцать пять сотых сантиметра"), 5.25, ::morphuntion::dialog::SpeakableString(u"сантиметр"));
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1,25 QQQ"), 1.25, ::morphuntion::dialog::SpeakableString(u"QQQ")); // nonsense
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2 QQQ"), 2, ::morphuntion::dialog::SpeakableString(u"QQQ")); // nonsense
}

TEST_CASE("QuantifyTest#testJapanese")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::JAPANESE());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1word"), 1, ::morphuntion::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::morphuntion::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testMandarin")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::CHINA());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1word"), 1, ::morphuntion::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::morphuntion::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}

TEST_CASE("QuantifyTest#testCantonese")
{
    auto const conceptFactory = getCommonConceptFactory(::morphuntion::util::LocaleUtils::HONGKONG_CANTONESE());
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"1word"), 1, ::morphuntion::dialog::SpeakableString(u"word"));
    ::std::unique_ptr<::morphuntion::dialog::InflectableStringConcept> fromPhrase(npc(toInflectableStringConcept(conceptFactory, ::morphuntion::dialog::SpeakableString(u"Word"))));
    fromPhrase->putConstraintByName(u"measure", u"Blah");
    assertQuantity(conceptFactory, ::morphuntion::dialog::SpeakableString(u"2BlahWord"), 2, fromPhrase.get());
}
