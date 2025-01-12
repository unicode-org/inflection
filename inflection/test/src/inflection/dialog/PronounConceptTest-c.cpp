/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/dialog/PronounConcept.h>
#include <inflection/dialog/DisplayValue.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <util/StringContainer.hpp>
#include <memory>

typedef ::inflection::util::AutoCRelease<IDSpeakableString*, iss_destroy> AutoSpeakableStringRelease;
typedef ::inflection::util::AutoCRelease<IDPronounConcept*, ipron_destroy> AutoPronounConceptRelease;

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

static void checkPrintedValue(const IDSemanticFeatureConcept* semanticFeatureConcept, std::u16string_view expected)
{
    auto error = U_ZERO_ERROR;
    AutoSpeakableStringRelease result(isfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> printedResult(result.value);
    REQUIRE(printedResult);
    CHECK(inflection::util::StringViewUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(expected));
}

TEST_CASE("PronounConceptTest-c#testBadArgument")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto garbage = u"garbage";
    ipron_createFromInitialPronoun(model, garbage, -1, &error); // This is an unrecognized pronoun to start from.
    checkForFailure(&error);

    // FYI don't have API to create models for unknown languages yet. So we can't request things like the Klingon SemanticFeatureModel.
    ipron_createWithDefaults(nullptr, nullptr, -1, &error); // We really need a model.
    checkForFailure(&error);

    ipron_destroy(ipron_createWithDefaults(model, nullptr, -1, &error)); // We need a model, but no dictionary is fine.
    REQUIRE(U_SUCCESS(error));

    ipron_createWithCustom(nullptr, nullptr, -1, nullptr, -1, &error); // We really need a model.
    checkForFailure(&error);

    IDDisplayValue_Constraint displayValueConstraints[] = {
            {garbage, garbage}};
    IDDisplayValue displayValue = {
            garbage,
            displayValueConstraints,
            -1, std::size(displayValueConstraints)
    };
    REQUIRE_FALSE(idv_isValidDisplayValue(model, &displayValue, &error));
    REQUIRE(U_SUCCESS(error));
    displayValue.constraintMap = nullptr;
    REQUIRE_FALSE(idv_isValidDisplayValue(model, &displayValue, &error));
    REQUIRE(U_SUCCESS(error));
    displayValue.constraintMapLen = 0;
    REQUIRE(idv_isValidDisplayValue(model, &displayValue, &error));
}

TEST_CASE("PronounConceptTest-c#testExistence")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    auto person = u"person";
    auto second = u"second";
    auto number = u"number";
    auto singular = u"singular";
    auto definiteness = u"definiteness";
    auto definite = u"definite";
    AutoPronounConceptRelease pronounConcept(ipron_createWithDefaults(model, nullptr, -1, &error));
    REQUIRE(U_SUCCESS(error));
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);
    REQUIRE(U_SUCCESS(error));
    isfc_putConstraintByName(pronounFeatureConcept, person, second, -1, &error);
    REQUIRE(U_SUCCESS(error));
    isfc_putConstraintByName(pronounFeatureConcept, number, singular, -1, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(isfc_isExists(pronounFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    AutoSpeakableStringRelease result(isfc_toSpeakableStringCopy(pronounFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"you"); // This is a little under specified

    isfc_putConstraintByName(pronounFeatureConcept, definiteness, definite, -1, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK_FALSE(isfc_isExists(pronounFeatureConcept, &error));
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"they");  // There is no definite form.
}

TEST_CASE("PronounConceptTest-c#testCustom")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    IDDisplayValue_Constraint displayValueConstraints[] = {
            {u"person", u"second"},
            {u"number", u"plural"},
            {u"case", u"nominative"}};
    IDDisplayValue displayValue = {
            u"y'all",
            displayValueConstraints,
            -1, std::size(displayValueConstraints)
    };
    REQUIRE(U_SUCCESS(error));
    AutoPronounConceptRelease pronounConcept(ipron_createWithCustom(model, &displayValue, 1, nullptr, -1, &error));
    REQUIRE(U_SUCCESS(error));
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);

    auto person = u"person";
    auto second = u"second";
    auto number = u"number";
    auto singular = u"singular";
    auto plural = u"plural";

    isfc_putConstraintByName(pronounFeatureConcept, person, second, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"y'all"); // This is a little under specified. So we're starting with the plural one.
    auto isCustom = ipron_isCustomMatch(pronounConcept.value, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(isCustom);

    isfc_putConstraintByName(pronounFeatureConcept, number, singular, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"you");
    isCustom = ipron_isCustomMatch(pronounConcept.value, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK_FALSE(isCustom);

    isfc_putConstraintByName(pronounFeatureConcept, number, plural, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"y'all");

    IDDisplayValue_Constraint constraints[] = {{u"number", u"singular"}};
    AutoPronounConceptRelease pronounConcept2(ipron_createWithCustom(model, &displayValue, 1, constraints, std::size(constraints), &error));
    REQUIRE(U_SUCCESS(error));
    auto pronounFeatureConcept2 = ipron_toSemanticFeatureConcept(pronounConcept2.value, &error);
    REQUIRE(U_SUCCESS(error));
    auto originalPronounFeatureConcept2 = ipron_toPronounConcept(pronounFeatureConcept2, &error);
    REQUIRE(U_SUCCESS(error));
    CHECK(pronounConcept2.value == originalPronounFeatureConcept2);

    isfc_putConstraintByName(pronounFeatureConcept2, person, second, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept2, u"you"); // This is a little under specified, but the defaults favor singular

    isfc_putConstraintByName(pronounFeatureConcept2, number, singular, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept2, u"you");

    isfc_putConstraintByName(pronounFeatureConcept2, number, plural, -1, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept2, u"y'all");
}

TEST_CASE("PronounConceptTest-c#testFullSupport")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    for (const auto& localeObject : inflection::util::LocaleUtils::getSupportedLocaleList()) {
        const char* locale = localeObject.getName().c_str();
        auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
        REQUIRE(U_SUCCESS(error));
        auto model = iccf_getSemanticFeatureModel(ccf, &error);
        REQUIRE(U_SUCCESS(error));

        auto pronounConcept = ipron_createWithDefaults(model, nullptr, -1, &error);
        REQUIRE(U_SUCCESS(error));

        iss_destroy(isfc_toSpeakableStringCopy(ipron_toSemanticFeatureConcept(pronounConcept, &error), &error));
        REQUIRE(U_SUCCESS(error));

        ipron_destroy(pronounConcept);
    }
}

TEST_CASE("PronounConceptTest-c#testDependency")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));
    const char* locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    REQUIRE(U_SUCCESS(error));
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    REQUIRE(U_SUCCESS(error));

    IDDisplayValue_Constraint defaultConstraints[] = {
            {u"case",   u"genitive"},
            {u"person", u"first"},
            {u"number", u"singular"},
    };
    AutoPronounConceptRelease pronounConcept(ipron_createWithDefaults(model, defaultConstraints, std::size(defaultConstraints), &error));
    REQUIRE(U_SUCCESS(error));
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);
    REQUIRE(U_SUCCESS(error));
    checkPrintedValue(pronounFeatureConcept, u"mío"); // This is a little under specified

    auto pronounConceptClone = isfc_clone(pronounFeatureConcept, &error);
    REQUIRE(U_SUCCESS(error));
    AutoSpeakableStringRelease result(isfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error));
    REQUIRE(U_SUCCESS(error));
    util::StringContainer<IDSpeakableString, iss_getPrint> printedResult(result.value);
    REQUIRE(printedResult);
    CHECK(inflection::util::StringViewUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(u"mío")); // This is a little under specified

    auto gender = u"gender";
    auto feminine = u"feminine";
    isfc_putConstraintByName(pronounConceptClone, gender, feminine, -1, &error);
    REQUIRE(U_SUCCESS(error));
    result = isfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error);
    REQUIRE(U_SUCCESS(error));
    printedResult.extract(result.value);
    REQUIRE(printedResult);
    CHECK(inflection::util::StringViewUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(u"mía"));

    isfc_destroy(pronounConceptClone);
}

TEST_CASE("PronounConceptTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    ipron_toPronounConcept(nullptr, &error);
    checkForFailure(&error);
    ipron_createWithCustom(nullptr, nullptr, -1, nullptr, -1, &error);
    checkForFailure(&error);
    ipron_createWithDefaults(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    ipron_createFromInitialPronoun(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    ipron_isCustomMatch(nullptr, &error);
    checkForFailure(&error);
}
