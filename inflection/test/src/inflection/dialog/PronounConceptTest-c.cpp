/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/dialog/PronounConcept.h>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <util/CFUtils.hpp>
#include <CoreFoundation/CFDictionary.h>
#include <memory>

typedef ::inflection::util::AutoCRelease<IDSpeakableString*, iss_destroy> AutoSpeakableStringRelease;
typedef ::inflection::util::AutoCRelease<IDPronounConcept*, ipron_destroy> AutoPronounConceptRelease;

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

static CFMutableDictionaryRef createConstraints(const std::map<std::u16string, std::u16string>& constraints) {
    CFMutableDictionaryRef semanticConstraints = CFDictionaryCreateMutable(kCFAllocatorDefault, (CFIndex) constraints.size(), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    for (const auto& [key, value] : constraints) {
        auto keyCF = util::TypeConversionUtils::to_CFString(key);
        auto valueCF = util::TypeConversionUtils::to_CFString(value);
        CFDictionaryAddValue(semanticConstraints, keyCF, valueCF);
        CFRelease(keyCF);
        CFRelease(valueCF);
    }
    return semanticConstraints;
}

static IDSemanticFeatureModel_DisplayValue* createDisplayValue(const IDSemanticFeatureModel* model, const std::u16string& displayValue, const std::map<std::u16string, std::u16string>& constraints, UErrorCode *error) {
    inflection::util::AutoCFRelease<CFStringRef> displayValueCF(util::TypeConversionUtils::to_CFString(displayValue));
    inflection::util::AutoCFRelease<CFMutableDictionaryRef> constraintsCF(createConstraints(constraints));
    return idv_create(model, displayValueCF.value, constraintsCF.value, error);
}

static void checkPrintedValue(const IDSemanticFeatureConcept* semanticFeatureConcept, std::u16string_view expected)
{
    auto error = U_ZERO_ERROR;
    AutoSpeakableStringRelease result(isfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> printedResult(iss_createPrintCopy(result.value, &error));
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(expected));
}

TEST_CASE("PronounConceptTest-c#testBadArgument")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    inflection::util::AutoCFRelease<CFStringRef> garbage(util::TypeConversionUtils::to_CFString(u"garbage"));
    ipron_createFromInitialPronoun(model, garbage.value, &error); // This is an unrecognized pronoun to start from.
    checkForFailure(&error);

    // FYI don't have API to create models for unknown languages yet. So we can't request things like the Klingon SemanticFeatureModel.
    ipron_createWithDefaults(nullptr, nullptr, &error); // We really need a model.
    checkForFailure(&error);

    ipron_destroy(ipron_createWithDefaults(model, nullptr, &error)); // We need a model, but no dictionary is fine.
    checkForSuccess(&error);

    ipron_createWithCustom(nullptr, nullptr, nullptr, &error); // We really need a model.
    checkForFailure(&error);

    createDisplayValue(model, u"garbage" , {{u"garbage", u"garbage"}}, &error);
    checkForFailure(&error);
}

TEST_CASE("PronounConceptTest-c#testExistence")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    auto person = u"person";
    inflection::util::AutoCFRelease<CFStringRef> second(util::TypeConversionUtils::to_CFString(u"second"));
    auto number = u"number";
    inflection::util::AutoCFRelease<CFStringRef> singular(util::TypeConversionUtils::to_CFString(u"singular"));
    auto definiteness = u"definiteness";
    inflection::util::AutoCFRelease<CFStringRef> definite(util::TypeConversionUtils::to_CFString(u"definite"));
    AutoPronounConceptRelease pronounConcept(ipron_createWithDefaults(model, nullptr, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);
    checkForSuccess(&error);
    isfc_putConstraintByName(pronounFeatureConcept, person, second.value, &error);
    checkForSuccess(&error);
    isfc_putConstraintByName(pronounFeatureConcept, number, singular.value, &error);
    checkForSuccess(&error);
    CHECK(isfc_isExists(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    AutoSpeakableStringRelease result(isfc_toSpeakableStringCopy(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"you"); // This is a little under specified

    isfc_putConstraintByName(pronounFeatureConcept, definiteness, definite.value, &error);
    checkForSuccess(&error);
    CHECK_FALSE(isfc_isExists(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"they");  // There is no definite form.
}

TEST_CASE("PronounConceptTest-c#testCustom")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    const char* locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    IDSemanticFeatureModel_DisplayValue* displayValue =
        createDisplayValue(model, u"y'all", {{u"person", u"second"}, {u"number", u"plural"}, {u"case", u"nominative"}}, &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFMutableArrayRef> defaultDisplayData(CFArrayCreateMutable(nullptr, 1, nullptr));
    CFArrayAppendValue(defaultDisplayData.value, displayValue);
    AutoPronounConceptRelease pronounConcept(ipron_createWithCustom(model, defaultDisplayData.value, nullptr, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);

    auto person = u"person";
    inflection::util::AutoCFRelease<CFStringRef> second(util::TypeConversionUtils::to_CFString(u"second"));
    auto number = u"number";
    inflection::util::AutoCFRelease<CFStringRef> singular(util::TypeConversionUtils::to_CFString(u"singular"));
    inflection::util::AutoCFRelease<CFStringRef> plural(util::TypeConversionUtils::to_CFString(u"plural"));

    isfc_putConstraintByName(pronounFeatureConcept, person, second.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"y'all"); // This is a little under specified. So we're starting with the plural one.
    auto isCustom = ipron_isCustomMatch(pronounConcept.value, &error);
    checkForSuccess(&error);
    CHECK(isCustom);

    isfc_putConstraintByName(pronounFeatureConcept, number, singular.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"you");
    isCustom = ipron_isCustomMatch(pronounConcept.value, &error);
    checkForSuccess(&error);
    CHECK_FALSE(isCustom);

    isfc_putConstraintByName(pronounFeatureConcept, number, plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"y'all");

    inflection::util::AutoCFRelease<CFMutableDictionaryRef> constraints(createConstraints({{u"number", u"singular"}}));
    AutoPronounConceptRelease pronounConcept2(ipron_createWithCustom(model, defaultDisplayData.value, constraints.value, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept2 = ipron_toSemanticFeatureConcept(pronounConcept2.value, &error);
    checkForSuccess(&error);
    auto originalPronounFeatureConcept2 = ipron_toPronounConcept(pronounFeatureConcept2, &error);
    checkForSuccess(&error);
    CHECK(pronounConcept2.value == originalPronounFeatureConcept2);

    isfc_putConstraintByName(pronounFeatureConcept2, person, second.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"you"); // This is a little under specified, but the defaults favor singular

    isfc_putConstraintByName(pronounFeatureConcept2, number, singular.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"you");

    isfc_putConstraintByName(pronounFeatureConcept2, number, plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"y'all");

    idv_destroy(displayValue);
}

TEST_CASE("PronounConceptTest-c#testFullSupport")
{
    auto error = U_ZERO_ERROR;
    inflection::util::AutoCFRelease<CFMutableDictionaryRef> emptyMap(CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    for (const auto& localeObject : inflection::util::LocaleUtils::getSupportedLocaleList()) {
        const char* locale = localeObject.getName().c_str();
        auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
        checkForSuccess(&error);
        auto model = iccf_getSemanticFeatureModel(ccf, &error);
        checkForSuccess(&error);

        auto pronounConcept = ipron_createWithDefaults(model, emptyMap.value, &error);
        checkForSuccess(&error);

        iss_destroy(isfc_toSpeakableStringCopy(ipron_toSemanticFeatureConcept(pronounConcept, &error), &error));
        checkForSuccess(&error);

        ipron_destroy(pronounConcept);
    }
}

TEST_CASE("PronounConceptTest-c#testDependency")
{
    auto error = U_ZERO_ERROR;
    auto commonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    const char* locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    auto ccf = ilccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale, &error);
    checkForSuccess(&error);
    auto model = iccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    inflection::util::AutoCFRelease<CFMutableDictionaryRef> defaultConstraints(createConstraints({
        {u"case", u"genitive"},
        {u"person", u"first"},
        {u"number", u"singular"},
    }));
    AutoPronounConceptRelease pronounConcept(ipron_createWithDefaults(model, defaultConstraints.value, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = ipron_toSemanticFeatureConcept(pronounConcept.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"mío"); // This is a little under specified

    auto pronounConceptClone = isfc_clone(pronounFeatureConcept, &error);
    checkForSuccess(&error);
    AutoSpeakableStringRelease result(isfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error));
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> printedResult(iss_createPrintCopy(result.value, &error));
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(u"mío")); // This is a little under specified

    auto gender = u"gender";
    inflection::util::AutoCFRelease<CFStringRef> feminine(util::TypeConversionUtils::to_CFString(u"feminine"));
    isfc_putConstraintByName(pronounConceptClone, gender, feminine.value, &error);
    checkForSuccess(&error);
    result = isfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error);
    checkForSuccess(&error);
    printedResult = iss_createPrintCopy(result.value, &error);
    checkForSuccess(&error);
    CHECK(util::TypeConversionUtils::to_string(printedResult.value) == inflection::util::StringViewUtils::to_string(u"mía"));

    isfc_destroy(pronounConceptClone);
}

TEST_CASE("PronounConceptTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    ipron_toPronounConcept(nullptr, &error);
    checkForFailure(&error);
    ipron_createWithCustom(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    ipron_createWithDefaults(nullptr, nullptr, &error);
    checkForFailure(&error);
    ipron_createFromInitialPronoun(nullptr, nullptr, &error);
    checkForFailure(&error);
    ipron_isCustomMatch(nullptr, &error);
    checkForFailure(&error);
}
