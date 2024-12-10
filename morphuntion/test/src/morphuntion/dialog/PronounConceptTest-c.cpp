/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/dialog/PronounConcept.h>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.h>
#include <morphuntion/dialog/SemanticFeatureConcept.h>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <util/TestUtils.hpp>
#include <util/CFUtils.hpp>
#include <CoreFoundation/CFDictionary.h>
#include <memory>
#include <set>

typedef ::morphuntion::util::AutoCRelease<MDSpeakableString*, mss_destroy> AutoSpeakableStringRelease;
typedef ::morphuntion::util::AutoCRelease<MDPronounConcept*, mpron_destroy> AutoPronounConceptRelease;

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

static CFMutableDictionaryRef createConstraints(const std::map<std::u16string, std::u16string>& constraints) {
    CFMutableDictionaryRef semanticConstraints = CFDictionaryCreateMutable(kCFAllocatorDefault, (CFIndex) constraints.size(), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    for (const auto& [key, value] : constraints) {
        auto keyCF = util::CFUtils::to_CFString(key);
        auto valueCF = util::CFUtils::to_CFString(value);
        CFDictionaryAddValue(semanticConstraints, keyCF, valueCF);
        CFRelease(keyCF);
        CFRelease(valueCF);
    }
    return semanticConstraints;
}

static MDSemanticFeatureModel_DisplayValue* createDisplayValue(const MDSemanticFeatureModel* model, const std::u16string& displayValue, const std::map<std::u16string, std::u16string>& constraints, CFErrorRef *error) {
    morphuntion::util::AutoCFRelease<CFStringRef> displayValueCF(util::CFUtils::to_CFString(displayValue));
    morphuntion::util::AutoCFRelease<CFMutableDictionaryRef> constraintsCF(createConstraints(constraints));
    return mdv_create(model, displayValueCF.value, constraintsCF.value, error);
}

static void checkPrintedValue(const MDSemanticFeatureConcept* semanticFeatureConcept, std::u16string_view expected)
{
    CFErrorRef error = nullptr;
    AutoSpeakableStringRelease result(msfc_toSpeakableStringCopy(semanticFeatureConcept, &error));
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> printedResult(mss_createPrintCopy(result.value, &error));
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_string(printedResult.value) == morphuntion::util::StringViewUtils::to_string(expected));
}

TEST_CASE("PronounConceptTest-c#testBadArgument")
{
    CFErrorRef error = nullptr;
    auto commonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto ccf = mlccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> garbage(util::CFUtils::to_CFString(u"garbage"));
    mpron_createFromInitialPronoun(model, garbage.value, &error); // This is an unrecognized pronoun to start from.
    checkForFailure(&error);

    // FYI don't have API to create models for unknown languages yet. So we can't request things like the Klingon SemanticFeatureModel.
    mpron_createWithDefaults(nullptr, nullptr, &error); // We really need a model.
    checkForFailure(&error);

    mpron_destroy(mpron_createWithDefaults(model, nullptr, &error)); // We need a model, but no dictionary is fine.
    checkForSuccess(&error);

    mpron_createWithCustom(nullptr, nullptr, nullptr, &error); // We really need a model.
    checkForFailure(&error);

    createDisplayValue(model, u"garbage" , {{u"garbage", u"garbage"}}, &error);
    checkForFailure(&error);
}

TEST_CASE("PronounConceptTest-c#testExistence")
{
    CFErrorRef error = nullptr;
    auto commonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto ccf = mlccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> person(util::CFUtils::to_CFString(u"person"));
    morphuntion::util::AutoCFRelease<CFStringRef> second(util::CFUtils::to_CFString(u"second"));
    morphuntion::util::AutoCFRelease<CFStringRef> count(util::CFUtils::to_CFString(u"number"));
    morphuntion::util::AutoCFRelease<CFStringRef> singular(util::CFUtils::to_CFString(u"singular"));
    morphuntion::util::AutoCFRelease<CFStringRef> definiteness(util::CFUtils::to_CFString(u"definiteness"));
    morphuntion::util::AutoCFRelease<CFStringRef> definite(util::CFUtils::to_CFString(u"definite"));
    AutoPronounConceptRelease pronounConcept(mpron_createWithDefaults(model, nullptr, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = mpron_toSemanticFeatureConcept(pronounConcept.value, &error);
    checkForSuccess(&error);
    msfc_putConstraintByName(pronounFeatureConcept, person.value, second.value, &error);
    checkForSuccess(&error);
    msfc_putConstraintByName(pronounFeatureConcept, count.value, singular.value, &error);
    checkForSuccess(&error);
    CHECK(msfc_isExists(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    AutoSpeakableStringRelease result(msfc_toSpeakableStringCopy(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"you"); // This is a little under specified

    msfc_putConstraintByName(pronounFeatureConcept, definiteness.value, definite.value, &error);
    checkForSuccess(&error);
    CHECK_FALSE(msfc_isExists(pronounFeatureConcept, &error));
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"they");  // There is no definite form.
}

TEST_CASE("PronounConceptTest-c#testCustom")
{
    CFErrorRef error = nullptr;
    auto commonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto ccf = mlccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    MDSemanticFeatureModel_DisplayValue* displayValue =
        createDisplayValue(model, u"y'all", {{u"person", u"second"}, {u"number", u"plural"}, {u"case", u"nominative"}}, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFMutableArrayRef> defaultDisplayData(CFArrayCreateMutable(nullptr, 1, nullptr));
    CFArrayAppendValue(defaultDisplayData.value, displayValue);
    AutoPronounConceptRelease pronounConcept(mpron_createWithCustom(model, defaultDisplayData.value, nullptr, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = mpron_toSemanticFeatureConcept(pronounConcept.value, &error);

    morphuntion::util::AutoCFRelease<CFStringRef> person(util::CFUtils::to_CFString(u"person"));
    morphuntion::util::AutoCFRelease<CFStringRef> second(util::CFUtils::to_CFString(u"second"));
    morphuntion::util::AutoCFRelease<CFStringRef> count(util::CFUtils::to_CFString(u"number"));
    morphuntion::util::AutoCFRelease<CFStringRef> singular(util::CFUtils::to_CFString(u"singular"));
    morphuntion::util::AutoCFRelease<CFStringRef> plural(util::CFUtils::to_CFString(u"plural"));

    msfc_putConstraintByName(pronounFeatureConcept, person.value, second.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"y'all"); // This is a little under specified. So we're starting with the plural one.
    auto isCustom = mpron_isCustomMatch(pronounConcept.value, &error);
    checkForSuccess(&error);
    CHECK(isCustom);

    msfc_putConstraintByName(pronounFeatureConcept, count.value, singular.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"you");
    isCustom = mpron_isCustomMatch(pronounConcept.value, &error);
    checkForSuccess(&error);
    CHECK_FALSE(isCustom);

    msfc_putConstraintByName(pronounFeatureConcept, count.value, plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"y'all");

    morphuntion::util::AutoCFRelease<CFMutableDictionaryRef> constraints(createConstraints({{u"number", u"singular"}}));
    AutoPronounConceptRelease pronounConcept2(mpron_createWithCustom(model, defaultDisplayData.value, constraints.value, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept2 = mpron_toSemanticFeatureConcept(pronounConcept2.value, &error);
    checkForSuccess(&error);
    auto originalPronounFeatureConcept2 = mpron_toPronounConcept(pronounFeatureConcept2, &error);
    checkForSuccess(&error);
    CHECK(pronounConcept2.value == originalPronounFeatureConcept2);

    msfc_putConstraintByName(pronounFeatureConcept2, person.value, second.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"you"); // This is a little under specified, but the defaults favor singular

    msfc_putConstraintByName(pronounFeatureConcept2, count.value, singular.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"you");

    msfc_putConstraintByName(pronounFeatureConcept2, count.value, plural.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept2, u"y'all");

    mdv_destroy(displayValue);
}

TEST_CASE("PronounConceptTest-c#testFullSupport")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFMutableDictionaryRef> emptyMap(CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
    auto commonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    for (const auto& localeObject : morphuntion::util::LocaleUtils::getSupportedLocaleList()) {
        morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(localeObject));
        auto ccf = mlccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale.value, &error);
        checkForSuccess(&error);
        auto model = mccf_getSemanticFeatureModel(ccf, &error);
        checkForSuccess(&error);

        auto pronounConcept = mpron_createWithDefaults(model, emptyMap.value, &error);
        checkForSuccess(&error);

        mss_destroy(msfc_toSpeakableStringCopy(mpron_toSemanticFeatureConcept(pronounConcept, &error), &error));
        checkForSuccess(&error);

        mpron_destroy(pronounConcept);
    }
}

TEST_CASE("PronounConceptTest-c#testDependency")
{
    CFErrorRef error = nullptr;
    auto commonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SPANISH()));
    auto ccf = mlccfp_getCommonConceptFactory(commonConceptFactoryProvider, locale.value, &error);
    checkForSuccess(&error);
    auto model = mccf_getSemanticFeatureModel(ccf, &error);
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFMutableDictionaryRef> defaultConstraints(createConstraints({
        {u"case", u"genitive"},
        {u"person", u"first"},
        {u"number", u"singular"},
    }));
    AutoPronounConceptRelease pronounConcept(mpron_createWithDefaults(model, defaultConstraints.value, &error));
    checkForSuccess(&error);
    auto pronounFeatureConcept = mpron_toSemanticFeatureConcept(pronounConcept.value, &error);
    checkForSuccess(&error);
    checkPrintedValue(pronounFeatureConcept, u"mío"); // This is a little under specified

    auto pronounConceptClone = msfc_clone(pronounFeatureConcept, &error);
    checkForSuccess(&error);
    AutoSpeakableStringRelease result(msfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error));
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> printedResult(mss_createPrintCopy(result.value, &error));
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_string(printedResult.value) == morphuntion::util::StringViewUtils::to_string(u"mío")); // This is a little under specified

    morphuntion::util::AutoCFRelease<CFStringRef> gender(util::CFUtils::to_CFString(u"gender"));
    morphuntion::util::AutoCFRelease<CFStringRef> feminine(util::CFUtils::to_CFString(u"feminine"));
    msfc_putConstraintByName(pronounConceptClone, gender.value, feminine.value, &error);
    checkForSuccess(&error);
    result = msfc_toSpeakableStringWithDependencyCopy(pronounFeatureConcept, pronounConceptClone, &error);
    checkForSuccess(&error);
    printedResult = mss_createPrintCopy(result.value, &error);
    checkForSuccess(&error);
    CHECK(util::CFUtils::to_string(printedResult.value) == morphuntion::util::StringViewUtils::to_string(u"mía"));

    msfc_destroy(pronounConceptClone);
}

TEST_CASE("PronounConceptTest-c#testError")
{
    CFErrorRef error = nullptr;

    mpron_toPronounConcept(nullptr, &error);
    checkForFailure(&error);
    mpron_createWithCustom(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    mpron_createWithDefaults(nullptr, nullptr, &error);
    checkForFailure(&error);
    mpron_createFromInitialPronoun(nullptr, nullptr, &error);
    checkForFailure(&error);
    mpron_isCustomMatch(nullptr, &error);
    checkForFailure(&error);
}
