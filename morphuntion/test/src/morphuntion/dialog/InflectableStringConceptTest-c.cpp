/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/InflectableStringConcept.h>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include "util/CFUtils.hpp"

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

static MDInflectableStringConcept* createConcept(const MDSemanticFeatureModel* model, const ::std::u16string &string, const ::std::vector<::std::u16string>& namedValues)
{
    CFErrorRef error = nullptr;

    morphuntion::util::AutoCFRelease<CFStringRef> cfsource(util::CFUtils::to_CFString(string));
    auto sssource = mss_create(cfsource.value, &error);
    checkForSuccess(&error);
    auto sssource2 = mss_createPrintSpeak(cfsource.value, cfsource.value, &error);
    checkForSuccess(&error);
    CHECK(mss_speakEqualsPrint(sssource2, &error));
    checkForSuccess(&error);
    CHECK(mss_contains(sssource, cfsource.value, &error));
    checkForSuccess(&error);
    mss_destroy(sssource2);


    auto result = minf_create(model, sssource, &error);
    mss_destroy(sssource);

    checkForSuccess(&error);
    minf_reset(result, &error);
    checkForSuccess(&error);
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::morphuntion::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                value = *iterator;
            }
        }
        morphuntion::util::AutoCFRelease<CFStringRef> cf_key(util::CFUtils::to_CFString(key));
        morphuntion::util::AutoCFRelease<CFStringRef> cf_value(util::CFUtils::to_CFString(value));
        minf_putConstraintByName(result, cf_key.value, cf_value.value, &error);
        checkForSuccess(&error);
        auto ss_value2 = minf_getFeatureValueByNameCopy(result, cf_key.value, &error);
        checkForSuccess(&error);
        morphuntion::util::AutoCFRelease<CFStringRef> cf_value2(mss_createPrintCopy(ss_value2, &error));
        checkForSuccess(&error);
        auto s_value = util::CFUtils::to_string(cf_value.value);
        auto s_value2 = util::CFUtils::to_string(cf_value2.value);
        CHECK(s_value == s_value2);
        INFO(::std::string("value: ") + s_value);
        INFO(::std::string("value2: ") + s_value2);
        mss_destroy(ss_value2);
    }
    return result;
}

static void compareInflection(const MDSemanticFeatureModel* model, const ::std::u16string& expected, const ::std::u16string& source, const ::std::vector<::std::u16string>& namedValues)
{
    CFErrorRef error = nullptr;
    auto inflectableConcept = createConcept(model, source, namedValues);

    auto result = minf_toSpeakableString(inflectableConcept, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> cfresult(mss_createPrintCopy(result, &error));
    checkForSuccess(&error);
    auto result_s = util::CFUtils::to_string(cfresult.value);
    auto expected_s = morphuntion::util::StringUtils::to_string(expected);
    INFO(::std::string("expected: ") + expected_s);
    INFO(::std::string("actual: ") + result_s);
    CHECK(expected_s == result_s);
    minf_destroy(inflectableConcept);
    mss_destroy(result);
}

static void checkPrintedValue(const MDSemanticFeatureConcept* semanticFeatureConcept, std::u16string_view expected)
{
    CFErrorRef error = nullptr;
    checkForSuccess(&error);
    auto result(msfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> printedResult(mss_createPrintCopy(result, &error));
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_u16string(printedResult.value) == expected);
    mss_destroy(result);
}

TEST_CASE("InflectableStringConceptTest-c#testExistsAPISpanish")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SPANISH()));
    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=neuter"});
    REQUIRE_FALSE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=common"});
    REQUIRE_FALSE(minf_isExists(inflectableConcept, &error));
    checkForFailure(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=masculine"});
    REQUIRE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"definiteness=definite"});
    REQUIRE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"gender=feminine"});
    REQUIRE_FALSE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"definiteness=definite"});
    REQUIRE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos abracadabra", {u"gender=feminine"});
    REQUIRE_FALSE(minf_isExists(inflectableConcept, &error));
    checkForSuccess(&error);
    minf_destroy(inflectableConcept);
}

TEST_CASE("InflectableStringConceptTest-c#testSpanish")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SPANISH()));

//    Optional creation method - aligns with InflectableStringConceptTest.cpp
    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);


    compareInflection(model, u"barco", u"barco", {});
    compareInflection(model, u"pan", u"pan", {});
    compareInflection(model, u"línea aérea", u"línea aérea", {});
    compareInflection(model, u"barco", u"barco", {u"number=singular"});
    compareInflection(model, u"pan", u"pan", {u"number=singular"});
    compareInflection(model, u"barcos", u"barco", {u"number=plural"});
    compareInflection(model, u"panes", u"pan", {u"number=plural"});
    compareInflection(model, u"paraguas", u"paraguas", {u"number=plural"});
    compareInflection(model, u"paracaídas", u"paracaídas", {u"number=plural"});
    compareInflection(model, u"precios históricos", u"precio histórico", {u"number=plural"});
    compareInflection(model, u"cambios porcentuales en tiempo real", u"cambio porcentual en tiempo real", {u"number=plural"});
    compareInflection(model, u"cambios de porcentaje", u"cambio de porcentaje", {u"number=plural"});
    compareInflection(model, u"faxes", u"fax", {u"number=plural"});
    compareInflection(model, u"hoteles", u"hotel", {u"number=plural"});
    compareInflection(model, u"lemmings", u"lemming", {u"number=plural"});
    compareInflection(model, u"lunes", u"lunes", {u"number=plural"});
    compareInflection(model, u"análisis", u"análisis", {u"number=plural"});
    compareInflection(model, u"apps de finanzas", u"app de finanzas", {u"number=plural"});
    compareInflection(model, u"el gato", u"gato", {u"definiteness=definite"});
    compareInflection(model, u"un gato", u"gato", {u"definiteness=indefinite"});
    compareInflection(model, u"la gata", u"gata", {u"definiteness=definite"});
    compareInflection(model, u"los gatos", u"gato", {u"definiteness=definite", u"number=plural"});
    compareInflection(model, u"las gatas", u"gato", {u"definiteness=definite", u"number=plural", u"gender=feminine"});
    compareInflection(model, u"las qwzqwces", u"qwzqwz", {u"definiteness=definite", u"number=plural", u"gender=feminine"}); // Made up word

    auto model2 = msfmod_create(locale.value, &error);
    checkForSuccess(&error);

    compareInflection(model2, u"toqueras", u"toquera", {u"number=plural"});
    compareInflection(model2, u"toqueros", u"toquero", {u"number=plural"});
    compareInflection(model2, u"BIENVENIDA", u"BIENVENIDA", {u"gender=feminine"});
    compareInflection(model2, u"BIENVENIDO", u"BIENVENIDO", {u"gender=masculine"});
    compareInflection(model2, u"BIENVENIDA", u"BIENVENIDO", {u"gender=feminine"});
    compareInflection(model2, u"BIENVENIDO", u"BIENVENIDA", {u"gender=masculine"});

    msfmod_destroy(model2);
}

TEST_CASE("InflectableStringConceptTest-c#testDanish")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::DANISH()));

    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"hund", {});
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> defArticle(util::CFUtils::to_CFString(u"defArticle"));
    auto featureValue = minf_getFeatureValueByNameCopy(inflectableConcept, defArticle.value, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> featureValuePrint(mss_createPrintCopy(featureValue, &error));
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> featureValueSpeak(mss_createSpeakCopy(featureValue, &error));
    checkForSuccess(&error);
    CHECK("den" == util::CFUtils::to_string(featureValuePrint.value));
    CHECK("dén" == util::CFUtils::to_string(featureValueSpeak.value));

    minf_destroy(inflectableConcept);
    mss_destroy(featureValue);
}

TEST_CASE("InflectableStringConceptTest-c#testClone")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));

    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"apple", {});

    morphuntion::util::AutoCFRelease<CFStringRef> definiteness(util::CFUtils::to_CFString(u"definiteness"));
    morphuntion::util::AutoCFRelease<CFStringRef> definite(util::CFUtils::to_CFString(u"definite"));
    minf_putConstraintByName(inflectableConcept, definiteness.value, definite.value, &error);
    checkForSuccess(&error);

    auto speakableString = minf_toSpeakableString(inflectableConcept, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> featureValuePrint(mss_createPrintCopy(speakableString, &error));
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> featureValueSpeak(mss_createSpeakCopy(speakableString, &error));
    checkForSuccess(&error);
    CHECK("the apple" == util::CFUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == util::CFUtils::to_string(featureValueSpeak.value));

    mss_destroy(speakableString);

    auto clonedConcept = minf_clone(inflectableConcept, &error);
    checkForSuccess(&error);

    speakableString = minf_toSpeakableString(clonedConcept, &error);
    checkForSuccess(&error);
    featureValuePrint = mss_createPrintCopy(speakableString, &error);
    checkForSuccess(&error);
    featureValueSpeak = mss_createSpeakCopy(speakableString, &error);
    checkForSuccess(&error);
    CHECK("the apple" == util::CFUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == util::CFUtils::to_string(featureValueSpeak.value));

    minf_clearConstraintByName(clonedConcept, definiteness.value, &error);
    checkForSuccess(&error);

    mss_destroy(speakableString);

    speakableString = minf_toSpeakableString(clonedConcept, &error);
    checkForSuccess(&error);
    featureValuePrint = mss_createPrintCopy(speakableString, &error);
    checkForSuccess(&error);
    featureValueSpeak = mss_createSpeakCopy(speakableString, &error);
    checkForSuccess(&error);
    CHECK("apple" == util::CFUtils::to_string(featureValuePrint.value));
    CHECK("apple" == util::CFUtils::to_string(featureValueSpeak.value));

    minf_destroy(inflectableConcept);
    minf_destroy(clonedConcept);
    mss_destroy(speakableString);
}

TEST_CASE("InflectableStringConceptTest-c#testConstraintManagement")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));

    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConceptOriginal = createConcept(model, u"test", {});
    checkForSuccess(&error);
    auto inflectableConcept = minf_toSemanticFeatureConcept(inflectableConceptOriginal, &error);
    checkForSuccess(&error);
    auto inflectableConceptBack = minf_toInflectableStringConcept(inflectableConcept, &error);
    checkForSuccess(&error);
    CHECK(inflectableConceptOriginal == inflectableConceptBack);

    morphuntion::util::AutoCFRelease<CFStringRef> count(util::CFUtils::to_CFString(u"number"));
    morphuntion::util::AutoCFRelease<CFStringRef> plural(util::CFUtils::to_CFString(u"plural"));
    morphuntion::util::AutoCFRelease<CFStringRef> constraint(msfc_createConstraintByNameCopy(inflectableConcept, count.value, &error));
    checkForSuccess(&error);
    CHECK(constraint.value == nullptr);

    checkPrintedValue(inflectableConcept, u"test");

    msfc_putConstraintByName(inflectableConcept, count.value, plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"tests");

    constraint = msfc_createConstraintByNameCopy(inflectableConcept, count.value, &error);
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_string(constraint.value) == "plural");

    morphuntion::util::AutoCFRelease<CFStringRef> caseStr(util::CFUtils::to_CFString(u"case"));
    morphuntion::util::AutoCFRelease<CFStringRef> genitive(util::CFUtils::to_CFString(u"genitive"));
    msfc_putConstraintByName(inflectableConcept, caseStr.value, genitive.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"tests’");

    auto inflectableConceptClone = msfc_clone(inflectableConcept, &error);
    checkForSuccess(&error);

    msfc_clearConstraintByName(inflectableConcept, count.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"test’s");

    constraint = msfc_createConstraintByNameCopy(inflectableConcept, count.value, &error);
    checkForSuccess(&error);
    CHECK(constraint.value == nullptr);

    msfc_reset(inflectableConcept, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"test");

    auto featureValue = msfc_createFeatureValueByNameCopy(inflectableConcept, count.value, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> featureValuePrint(mss_createPrintCopy(featureValue, &error));
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_string(featureValuePrint.value) == "singular");
    mss_destroy(featureValue);
    featureValue = nullptr;

    checkPrintedValue(inflectableConceptClone, u"tests’");

    morphuntion::util::AutoCFRelease<CFStringRef> garbage(util::CFUtils::to_CFString(u"garbage"));
    constraint = msfc_createConstraintByNameCopy(inflectableConcept, garbage.value, &error);
    checkForFailure(&error);
    CHECK(constraint.value == nullptr);
    featureValue = msfc_createFeatureValueByNameCopy(inflectableConcept, garbage.value, &error);
    checkForFailure(&error);
    CHECK(featureValue == nullptr);
    msfc_clearConstraintByName(inflectableConcept, garbage.value, &error);
    checkForFailure(&error);
    msfc_putConstraintByName(inflectableConcept, garbage.value, garbage.value, &error);
    checkForFailure(&error);

    msfc_destroy(inflectableConcept);
    msfc_destroy(inflectableConceptClone);
}

TEST_CASE("InflectableStringConceptTest-c#testError")
{
    CFErrorRef error = nullptr;

    mlccfp_getCommonConceptFactory(nullptr, nullptr, &error);
    checkForFailure(&error);
    mccf_getSemanticFeatureModel(nullptr, &error);
    checkForFailure(&error);

    msfmod_create(nullptr, &error);
    checkForFailure(&error);
    // Don't crash
    msfmod_destroy(nullptr);

    mss_create(nullptr, &error);
    checkForFailure(&error);
    mss_createPrintSpeak(nullptr, nullptr, &error);
    checkForFailure(&error);
    mss_speakEqualsPrint(nullptr, &error);
    checkForFailure(&error);
    mss_contains(nullptr, nullptr, &error);
    checkForFailure(&error);
    minf_toSpeakableString(nullptr, &error);
    checkForFailure(&error);
    mss_createPrintCopy(nullptr, &error);
    checkForFailure(&error);
    minf_destroy(nullptr);
    // Don't crash
    mss_destroy(nullptr);

    minf_create(nullptr, nullptr, &error);
    checkForFailure(&error);
    minf_reset(nullptr, &error);
    checkForFailure(&error);
    minf_putConstraintByName(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    minf_getFeatureValueByNameCopy(nullptr, nullptr, &error);
    checkForFailure(&error);
    mss_createPrintCopy(nullptr, &error);
    checkForFailure(&error);
    mss_createSpeakCopy(nullptr, &error);
    checkForFailure(&error);
    minf_clone(nullptr, &error);
    checkForFailure(&error);
    minf_clearConstraintByName(nullptr, nullptr, &error);
    checkForFailure(&error);

    msfc_reset(nullptr, &error);
    checkForFailure(&error);
    msfc_isExists(nullptr, &error);
    checkForFailure(&error);
    msfc_clone(nullptr, &error);
    checkForFailure(&error);
    msfc_toSpeakableStringCopy(nullptr, &error);
    checkForFailure(&error);
    msfc_toSpeakableStringWithDependencyCopy(nullptr, nullptr, &error);
    checkForFailure(&error);
    minf_toInflectableStringConcept(nullptr, &error);
    checkForFailure(&error);
}

