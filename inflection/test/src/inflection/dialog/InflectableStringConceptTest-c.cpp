/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/InflectableStringConcept.h>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include "util/CFUtils.hpp"

static void checkForSuccess(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_SUCCESS(*status));
}

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

static IDInflectableStringConcept* createConcept(const IDSemanticFeatureModel* model, const ::std::u16string &string, const ::std::vector<::std::u16string>& namedValues)
{
    auto error = U_ZERO_ERROR;

    inflection::util::AutoCFRelease<CFStringRef> cfsource(util::TypeConversionUtils::to_CFString(string));
    auto sssource = iss_create(cfsource.value, &error);
    checkForSuccess(&error);
    auto sssource2 = iss_createPrintSpeak(cfsource.value, cfsource.value, &error);
    checkForSuccess(&error);
    CHECK(iss_speakEqualsPrint(sssource2, &error));
    checkForSuccess(&error);
    CHECK(iss_contains(sssource, cfsource.value, &error));
    checkForSuccess(&error);
    iss_destroy(sssource2);


    auto result = iinf_create(model, sssource, &error);
    iss_destroy(sssource);
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(result, &error);

    checkForSuccess(&error);
    isfc_reset(semanticFeatureConcept, &error);
    checkForSuccess(&error);
    for (const auto& namedValue : namedValues) {
        ::std::u16string key;
        ::std::u16string value;
        for (::inflection::util::DelimitedStringIterator iterator(namedValue, u"="); iterator.hasNext(); ++iterator) {
            if (key.empty()) {
                key = *iterator;
            }
            else {
                value = *iterator;
            }
        }
        inflection::util::AutoCFRelease<CFStringRef> cf_value(util::TypeConversionUtils::to_CFString(value));
        isfc_putConstraintByName(semanticFeatureConcept, key.c_str(), cf_value.value, &error);
        checkForSuccess(&error);
        auto ss_value2 = isfc_createFeatureValueByNameCopy(semanticFeatureConcept, key.c_str(), &error);
        checkForSuccess(&error);
        inflection::util::AutoCFRelease<CFStringRef> cf_value2(iss_createPrintCopy(ss_value2, &error));
        checkForSuccess(&error);
        auto s_value = util::TypeConversionUtils::to_string(cf_value.value);
        auto s_value2 = util::TypeConversionUtils::to_string(cf_value2.value);
        CHECK(s_value == s_value2);
        INFO(::std::string("value: ") + s_value);
        INFO(::std::string("value2: ") + s_value2);
        iss_destroy(ss_value2);
    }
    return result;
}

static void compareInflection(const IDSemanticFeatureModel* model, const ::std::u16string& expected, const ::std::u16string& source, const ::std::vector<::std::u16string>& namedValues)
{
    auto error = U_ZERO_ERROR;
    auto inflectableConcept = createConcept(model, source, namedValues);
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);

    auto result = isfc_toSpeakableStringCopy(semanticFeatureConcept, &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> cfresult(iss_createPrintCopy(result, &error));
    checkForSuccess(&error);
    auto result_s = util::TypeConversionUtils::to_string(cfresult.value);
    auto expected_s = inflection::util::StringUtils::to_string(expected);
    INFO(::std::string("expected: ") + expected_s);
    INFO(::std::string("actual: ") + result_s);
    CHECK(expected_s == result_s);
    iinf_destroy(inflectableConcept);
    iss_destroy(result);
}

static void checkPrintedValue(const IDSemanticFeatureConcept* semanticFeatureConcept, std::u16string_view expected)
{
    auto error = U_ZERO_ERROR;
    checkForSuccess(&error);
    auto result(isfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> printedResult(iss_createPrintCopy(result, &error));
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_u16string(printedResult.value) == expected);
    iss_destroy(result);
}

TEST_CASE("InflectableStringConceptTest-c#testExistsAPISpanish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=neuter"});
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=common"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    checkForFailure(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=masculine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"definiteness=definite"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"gender=feminine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"definiteness=definite"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos abracadabra", {u"gender=feminine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    iinf_destroy(inflectableConcept);
}

TEST_CASE("InflectableStringConceptTest-c#testSpanish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();

//    Optional creation method - aligns with InflectableStringConceptTest.cpp
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
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

    auto model2 = isfmod_create(locale, &error);
    checkForSuccess(&error);

    compareInflection(model2, u"toqueras", u"toquera", {u"number=plural"});
    compareInflection(model2, u"toqueros", u"toquero", {u"number=plural"});
    compareInflection(model2, u"BIENVENIDA", u"BIENVENIDA", {u"gender=feminine"});
    compareInflection(model2, u"BIENVENIDO", u"BIENVENIDO", {u"gender=masculine"});
    compareInflection(model2, u"BIENVENIDA", u"BIENVENIDO", {u"gender=feminine"});
    compareInflection(model2, u"BIENVENIDO", u"BIENVENIDA", {u"gender=masculine"});

    isfmod_destroy(model2);
}

TEST_CASE("InflectableStringConceptTest-c#testDanish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::DANISH().getName().c_str();

    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"hund", {});
    checkForSuccess(&error);
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    checkForSuccess(&error);

    auto featureValue = isfc_createFeatureValueByNameCopy(semanticFeatureConcept, u"defArticle", &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> featureValuePrint(iss_createPrintCopy(featureValue, &error));
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> featureValueSpeak(iss_createSpeakCopy(featureValue, &error));
    checkForSuccess(&error);
    CHECK("den" == util::TypeConversionUtils::to_string(featureValuePrint.value));
    CHECK("dén" == util::TypeConversionUtils::to_string(featureValueSpeak.value));

    iinf_destroy(inflectableConcept);
    iss_destroy(featureValue);
}

TEST_CASE("InflectableStringConceptTest-c#testClone")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();

    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConcept = createConcept(model, u"apple", {});
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);

    inflection::util::AutoCFRelease<CFStringRef> definite(util::TypeConversionUtils::to_CFString(u"definite"));
    isfc_putConstraintByName(semanticFeatureConcept, u"definiteness", definite.value, &error);
    checkForSuccess(&error);

    auto speakableString = isfc_toSpeakableStringCopy(semanticFeatureConcept, &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> featureValuePrint(iss_createPrintCopy(speakableString, &error));
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> featureValueSpeak(iss_createSpeakCopy(speakableString, &error));
    checkForSuccess(&error);
    CHECK("the apple" == util::TypeConversionUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == util::TypeConversionUtils::to_string(featureValueSpeak.value));

    iss_destroy(speakableString);

    auto clonedConcept = isfc_clone(semanticFeatureConcept, &error);
    checkForSuccess(&error);

    speakableString = isfc_toSpeakableStringCopy(clonedConcept, &error);
    checkForSuccess(&error);
    featureValuePrint = iss_createPrintCopy(speakableString, &error);
    checkForSuccess(&error);
    featureValueSpeak = iss_createSpeakCopy(speakableString, &error);
    checkForSuccess(&error);
    CHECK("the apple" == util::TypeConversionUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == util::TypeConversionUtils::to_string(featureValueSpeak.value));

    isfc_clearConstraintByName(clonedConcept, u"definiteness", &error);
    checkForSuccess(&error);

    iss_destroy(speakableString);

    speakableString = isfc_toSpeakableStringCopy(clonedConcept, &error);
    checkForSuccess(&error);
    featureValuePrint = iss_createPrintCopy(speakableString, &error);
    checkForSuccess(&error);
    featureValueSpeak = iss_createSpeakCopy(speakableString, &error);
    checkForSuccess(&error);
    CHECK("apple" == util::TypeConversionUtils::to_string(featureValuePrint.value));
    CHECK("apple" == util::TypeConversionUtils::to_string(featureValueSpeak.value));

    iinf_destroy(inflectableConcept);
    isfc_destroy(clonedConcept);
    iss_destroy(speakableString);
}

TEST_CASE("InflectableStringConceptTest-c#testConstraintManagement")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();

    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto inflectableConceptOriginal = createConcept(model, u"test", {});
    checkForSuccess(&error);
    auto inflectableConcept = iinf_toSemanticFeatureConcept(inflectableConceptOriginal, &error);
    checkForSuccess(&error);
    auto inflectableConceptBack = iinf_toInflectableStringConcept(inflectableConcept, &error);
    checkForSuccess(&error);
    CHECK(inflectableConceptOriginal == inflectableConceptBack);

    inflection::util::AutoCFRelease<CFStringRef> plural(util::TypeConversionUtils::to_CFString(u"plural"));
    inflection::util::AutoCFRelease<CFStringRef> constraint(isfc_createConstraintByNameCopy(inflectableConcept, u"number", &error));
    checkForSuccess(&error);
    CHECK(constraint.value == nullptr);

    checkPrintedValue(inflectableConcept, u"test");

    isfc_putConstraintByName(inflectableConcept, u"number", plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"tests");

    constraint = isfc_createConstraintByNameCopy(inflectableConcept, u"number", &error);
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_string(constraint.value) == "plural");

    inflection::util::AutoCFRelease<CFStringRef> genitive(util::TypeConversionUtils::to_CFString(u"genitive"));
    isfc_putConstraintByName(inflectableConcept, u"case", genitive.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"tests’");

    auto inflectableConceptClone = isfc_clone(inflectableConcept, &error);
    checkForSuccess(&error);

    isfc_clearConstraintByName(inflectableConcept, u"number", &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"test’s");

    constraint = isfc_createConstraintByNameCopy(inflectableConcept, u"number", &error);
    checkForSuccess(&error);
    CHECK(constraint.value == nullptr);

    isfc_reset(inflectableConcept, &error);
    checkForSuccess(&error);
    checkPrintedValue(inflectableConcept, u"test");

    auto featureValue = isfc_createFeatureValueByNameCopy(inflectableConcept, u"number", &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> featureValuePrint(iss_createPrintCopy(featureValue, &error));
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_string(featureValuePrint.value) == "singular");
    iss_destroy(featureValue);
    featureValue = nullptr;

    checkPrintedValue(inflectableConceptClone, u"tests’");

    inflection::util::AutoCFRelease<CFStringRef> garbage(util::TypeConversionUtils::to_CFString(u"garbage"));
    constraint = isfc_createConstraintByNameCopy(inflectableConcept, u"garbage", &error);
    checkForFailure(&error);
    CHECK(constraint.value == nullptr);
    featureValue = isfc_createFeatureValueByNameCopy(inflectableConcept, u"garbage", &error);
    checkForFailure(&error);
    CHECK(featureValue == nullptr);
    isfc_clearConstraintByName(inflectableConcept, u"garbage", &error);
    checkForFailure(&error);
    isfc_putConstraintByName(inflectableConcept, u"garbage", garbage.value, &error);
    checkForFailure(&error);

    isfc_destroy(inflectableConcept);
    isfc_destroy(inflectableConceptClone);
}

TEST_CASE("InflectableStringConceptTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    ilccfp_getCommonConceptFactory(nullptr, nullptr, &error);
    checkForFailure(&error);
    iccf_getSemanticFeatureModel(nullptr, &error);
    checkForFailure(&error);

    isfmod_create(nullptr, &error);
    checkForFailure(&error);
    // Don't crash
    isfmod_destroy(nullptr);

    iss_create(nullptr, &error);
    checkForFailure(&error);
    iss_createPrintSpeak(nullptr, nullptr, &error);
    checkForFailure(&error);
    iss_speakEqualsPrint(nullptr, &error);
    checkForFailure(&error);
    iss_contains(nullptr, nullptr, &error);
    checkForFailure(&error);
    iss_createPrintCopy(nullptr, &error);
    checkForFailure(&error);
    iinf_destroy(nullptr);
    // Don't crash
    iss_destroy(nullptr);

    iinf_create(nullptr, nullptr, &error);
    checkForFailure(&error);
    isfc_putConstraintByName(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    isfc_createFeatureValueByNameCopy(nullptr, nullptr, &error);
    checkForFailure(&error);
    iss_createPrintCopy(nullptr, &error);
    checkForFailure(&error);
    iss_createSpeakCopy(nullptr, &error);
    checkForFailure(&error);
    isfc_clearConstraintByName(nullptr, nullptr, &error);
    checkForFailure(&error);

    isfc_reset(nullptr, &error);
    checkForFailure(&error);
    isfc_isExists(nullptr, &error);
    checkForFailure(&error);
    isfc_clone(nullptr, &error);
    checkForFailure(&error);
    isfc_toSpeakableStringCopy(nullptr, &error);
    checkForFailure(&error);
    isfc_toSpeakableStringWithDependencyCopy(nullptr, nullptr, &error);
    checkForFailure(&error);
    iinf_toInflectableStringConcept(nullptr, &error);
    checkForFailure(&error);}

