/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/AutoCFRelease.hpp>
#include "util/CFUtils.hpp"
#include <morphuntion/npc.hpp>

typedef ::morphuntion::util::AutoCRelease<MDSpeakableString*, &mss_destroy> AutoSpeakableStringRelease;

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

const MDCommonConceptFactory* getCommonConceptFactory_Quote(CFLocaleRef language)
{
    CFErrorRef error = nullptr;
    auto ccfp = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = mlccfp_getCommonConceptFactory(ccfp, language, &error);
    checkForSuccess(&error);
    return ccf;
}

static void compareSpeakableStrings(const MDSpeakableString* expected, const MDSpeakableString* actual) {
    auto expectedDesciptiveString = ::morphuntion::util::StringUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::morphuntion::util::StringUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::morphuntion::dialog::SpeakableString*) expected) == *((const ::morphuntion::dialog::SpeakableString*) actual));
}

static void runTestCase(const MDCommonConceptFactory *conceptFactory, const morphuntion::dialog::SpeakableString quotedExpected, const morphuntion::dialog::SpeakableString input) {
    CFErrorRef error = nullptr;
    AutoSpeakableStringRelease quotedActual(mccf_quote(conceptFactory, (MDSpeakableString *) &input, &error));
    checkForSuccess(&error);
    compareSpeakableStrings((MDSpeakableString *) &quotedExpected, quotedActual.value);
}

TEST_CASE("QuoteTest-c#testQuotePt")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::PORTUGAL()));
    auto conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory,::morphuntion::dialog::SpeakableString(u"«example»"),::morphuntion::dialog::SpeakableString(u"example"));
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"«example»", u"foobar"), ::morphuntion::dialog::SpeakableString(u"example", u"foobar"));
    cfLocale = ::util::CFUtils::convert(::morphuntion::util::LocaleUtils::BRAZIL());
    conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"“example”"), ::morphuntion::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteFr")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::FRANCE()));
    auto conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"“example”"), ::morphuntion::dialog::SpeakableString(u"example"));
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"“example”", u"exampel"), ::morphuntion::dialog::SpeakableString(u"example", u"exampel"));
    cfLocale = ::util::CFUtils::convert(::morphuntion::util::LocaleUtils::CANADIAN_FRENCH());
    conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"“example”"), ::morphuntion::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteIt")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ITALIAN()));
    auto conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"«example»"), ::morphuntion::dialog::SpeakableString(u"example"));
    cfLocale = ::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN());
    conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"«example»"), ::morphuntion::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteDe")
{
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::GERMANY()));
    auto conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"„example“"), ::morphuntion::dialog::SpeakableString(u"example"));
    cfLocale = ::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN());
    conceptFactory = getCommonConceptFactory_Quote(cfLocale.value);
    runTestCase(conceptFactory, ::morphuntion::dialog::SpeakableString(u"«example»"), ::morphuntion::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testError")
{
    CFErrorRef error = nullptr;

    mccf_quote(nullptr, nullptr, &error);
    checkForFailure(&error);
}
