/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/InflectableStringConcept.h>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include "util/StringContainer.hpp"

using ::util::StringContainer;

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

static IDInflectableStringConcept* createConcept(const IDSemanticFeatureModel* model, const ::std::u16string &string, const ::std::vector<::std::u16string>& namedValues)
{
    auto error = U_ZERO_ERROR;

    auto sssource = iss_create(string.c_str(), int32_t(string.length()), &error);
    REQUIRE(U_SUCCESS(error));
    auto sssource2 = iss_createPrintSpeak(string.c_str(), int32_t(string.length()), string.c_str(), int32_t(string.length()), &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(iss_speakEqualsPrint(sssource2, &error));
    REQUIRE(U_SUCCESS(error));
    CHECK(iss_contains(sssource, string.c_str(), int32_t(string.length()), &error));
    REQUIRE(U_SUCCESS(error));
    iss_destroy(sssource2);


    auto result = iinf_create(model, sssource, &error);
    iss_destroy(sssource);
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(result, &error);

    REQUIRE(U_SUCCESS(error));
    isfc_reset(semanticFeatureConcept, &error);
    REQUIRE(U_SUCCESS(error));
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
        isfc_putConstraintByName(semanticFeatureConcept, key.c_str(), value.c_str(), int32_t(value.length()), &error);
        REQUIRE(U_SUCCESS(error));
        auto ss_value2 = isfc_createFeatureValueByNameCopy(semanticFeatureConcept, key.c_str(), &error);
        REQUIRE(U_SUCCESS(error));
        util::StringContainer<IDSpeakableString, iss_getPrint> strValue2(ss_value2);
        REQUIRE(U_SUCCESS(error));
        auto s_value = inflection::util::StringUtils::to_string(value);
        auto s_value2 = inflection::util::StringUtils::to_string(strValue2.value);
        INFO(::std::string("value: ") + s_value);
        INFO(::std::string("value2: ") + s_value2);
        CHECK(s_value == s_value2);
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
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> printResult(result);
    REQUIRE(U_SUCCESS(error));
    auto result_s = inflection::util::StringUtils::to_string(printResult.value);
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
    REQUIRE(U_SUCCESS(error));
    auto result(isfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> printedResult(result);
    REQUIRE(U_SUCCESS(error));
    CHECK(printedResult.value == expected);
    iss_destroy(result);
}

TEST_CASE("InflectableStringConceptTest-c#testExistsAPISpanish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=neuter"});
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=common"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    checkForFailure(&error);
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"gender=masculine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos", {u"definiteness=definite"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"gender=feminine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"abracadabra", {u"definiteness=definite"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);

    inflectableConcept = createConcept(model, u"bienvenidos abracadabra", {u"gender=feminine"});
    semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE_FALSE(isfc_isExists(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    iinf_destroy(inflectableConcept);
}

TEST_CASE("InflectableStringConceptTest-c#testSpanish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();

//    Optional creation method - aligns with InflectableStringConceptTest.cpp
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));


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
    REQUIRE(U_SUCCESS(error));

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
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto inflectableConcept = createConcept(model, u"hund", {});
    REQUIRE(U_SUCCESS(error));
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);
    REQUIRE(U_SUCCESS(error));

    auto featureValue = isfc_createFeatureValueByNameCopy(semanticFeatureConcept, u"defArticle", &error);
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> featureValuePrint(featureValue);
    REQUIRE(featureValuePrint);
    util::StringContainer<IDSpeakableString, iss_getSpeak> featureValueSpeak(featureValue);
    REQUIRE(featureValuePrint);
    CHECK("den" == inflection::util::StringUtils::to_string(featureValuePrint.value));
    CHECK("dén" == inflection::util::StringUtils::to_string(featureValueSpeak.value));

    iinf_destroy(inflectableConcept);
    iss_destroy(featureValue);
}

TEST_CASE("InflectableStringConceptTest-c#testClone")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();

    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto inflectableConcept = createConcept(model, u"apple", {});
    auto semanticFeatureConcept = iinf_toSemanticFeatureConcept(inflectableConcept, &error);

    isfc_putConstraintByName(semanticFeatureConcept, u"definiteness", u"definite", -1, &error);
    REQUIRE(U_SUCCESS(error));

    auto speakableString = isfc_toSpeakableStringCopy(semanticFeatureConcept, &error);
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> featureValuePrint(speakableString);
    REQUIRE(featureValuePrint);
    util::StringContainer<IDSpeakableString, iss_getSpeak> featureValueSpeak(speakableString);
    REQUIRE(featureValuePrint);
    CHECK("the apple" == inflection::util::StringUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == inflection::util::StringUtils::to_string(featureValueSpeak.value));

    iss_destroy(speakableString);

    auto clonedConcept = isfc_clone(semanticFeatureConcept, &error);
    REQUIRE(U_SUCCESS(error));

    speakableString = isfc_toSpeakableStringCopy(clonedConcept, &error);
    REQUIRE(U_SUCCESS(error));
    featureValuePrint.extract(speakableString);
    REQUIRE(U_SUCCESS(error));
    featureValueSpeak.extract(speakableString);
    REQUIRE(U_SUCCESS(error));
    CHECK("the apple" == inflection::util::StringUtils::to_string(featureValuePrint.value));
    CHECK("the apple" == inflection::util::StringUtils::to_string(featureValueSpeak.value));

    isfc_clearConstraintByName(clonedConcept, u"definiteness", &error);
    REQUIRE(U_SUCCESS(error));

    iss_destroy(speakableString);

    speakableString = isfc_toSpeakableStringCopy(clonedConcept, &error);
    REQUIRE(U_SUCCESS(error));
    featureValuePrint.extract(speakableString);
    REQUIRE(U_SUCCESS(error));
    featureValueSpeak.extract(speakableString);
    REQUIRE(U_SUCCESS(error));
    CHECK("apple" == inflection::util::StringUtils::to_string(featureValuePrint.value));
    CHECK("apple" == inflection::util::StringUtils::to_string(featureValueSpeak.value));

    iinf_destroy(inflectableConcept);
    isfc_destroy(clonedConcept);
    iss_destroy(speakableString);
}

TEST_CASE("InflectableStringConceptTest-c#testConstraintManagement")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();

    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto inflectableConceptOriginal = createConcept(model, u"test", {});
    REQUIRE(U_SUCCESS(error));
    auto inflectableConcept = iinf_toSemanticFeatureConcept(inflectableConceptOriginal, &error);
    REQUIRE(U_SUCCESS(error));
    auto inflectableConceptBack = iinf_toInflectableStringConcept(inflectableConcept, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(inflectableConceptOriginal == inflectableConceptBack);

    auto plural = u"plural";
    char16_t constraint[32] = {};
    auto size = isfc_getConstraintByName(inflectableConcept, u"number", constraint, std::size(constraint), &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(size < 0);

    checkPrintedValue(inflectableConcept, u"test");

    isfc_putConstraintByName(inflectableConcept, u"number", plural, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(inflectableConcept, u"tests");

    isfc_getConstraintByName(inflectableConcept, u"number", constraint, std::size(constraint), &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(std::u16string(constraint) == u"plural");

    auto genitive = u"genitive";
    isfc_putConstraintByName(inflectableConcept, u"case", genitive, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(inflectableConcept, u"tests’");

    auto inflectableConceptClone = isfc_clone(inflectableConcept, &error);
    REQUIRE(U_SUCCESS(error));

    isfc_clearConstraintByName(inflectableConcept, u"number", &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(inflectableConcept, u"test’s");

    size = isfc_getConstraintByName(inflectableConcept, u"number", constraint, std::size(constraint), &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(size < 0);

    isfc_reset(inflectableConcept, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(inflectableConcept, u"test");

    auto featureValue = isfc_createFeatureValueByNameCopy(inflectableConcept, u"number", &error);
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> featureValuePrint(featureValue);
    REQUIRE(featureValuePrint);
    CHECK(inflection::util::StringUtils::to_string(featureValuePrint.value) == "singular");
    iss_destroy(featureValue);
    featureValue = nullptr;

    checkPrintedValue(inflectableConceptClone, u"tests’");

    auto garbage = u"garbage";
    size = isfc_getConstraintByName(inflectableConcept, garbage, constraint, std::size(constraint), &error);
    checkForFailure(&error);
    CHECK(size < 0);
    featureValue = isfc_createFeatureValueByNameCopy(inflectableConcept, garbage, &error);
    checkForFailure(&error);
    CHECK(featureValue == nullptr);
    isfc_clearConstraintByName(inflectableConcept, garbage, &error);
    checkForFailure(&error);
    isfc_putConstraintByName(inflectableConcept, garbage, garbage, -1, &error);
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

    iss_create(nullptr, -1, &error);
    checkForFailure(&error);
    iss_createPrintSpeak(nullptr, -1, nullptr, -1, &error);
    checkForFailure(&error);
    iss_speakEqualsPrint(nullptr, &error);
    checkForFailure(&error);
    iss_contains(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    iss_getPrint(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    iinf_destroy(nullptr);
    // Don't crash
    iss_destroy(nullptr);

    iinf_create(nullptr, nullptr, &error);
    checkForFailure(&error);
    isfc_putConstraintByName(nullptr, nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    isfc_createFeatureValueByNameCopy(nullptr, nullptr, &error);
    checkForFailure(&error);
    iss_getPrint(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    iss_getSpeak(nullptr, nullptr, -1, &error);
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
    checkForFailure(&error);
}
