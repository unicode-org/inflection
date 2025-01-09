/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/npc.hpp>

typedef ::inflection::util::AutoCRelease<IDSpeakableString*, &iss_destroy> AutoSpeakableStringRelease;

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


const IDCommonConceptFactory* getCommonConceptFactory_Quote(const char* language)
{
    auto error = U_ZERO_ERROR;
    auto ccfp = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);
    auto ccf = ilccfp_getCommonConceptFactory(ccfp, language, &error);
    checkForSuccess(&error);
    return ccf;
}

static void compareSpeakableStrings(const IDSpeakableString* expected, const IDSpeakableString* actual) {
    auto expectedDesciptiveString = ::inflection::util::StringUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::inflection::util::StringUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::inflection::dialog::SpeakableString*) expected) == *((const ::inflection::dialog::SpeakableString*) actual));
}

static void runTestCase(const IDCommonConceptFactory *conceptFactory, const inflection::dialog::SpeakableString quotedExpected, const inflection::dialog::SpeakableString input) {
    auto error = U_ZERO_ERROR;
    AutoSpeakableStringRelease quotedActual(iccf_quote(conceptFactory, (IDSpeakableString *) &input, &error));
    checkForSuccess(&error);
    compareSpeakableStrings((IDSpeakableString *) &quotedExpected, quotedActual.value);
}

TEST_CASE("QuoteTest-c#testQuotePt")
{
    auto locale = ::inflection::util::LocaleUtils::PORTUGAL().getName().c_str();
    auto conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory,::inflection::dialog::SpeakableString(u"«example»"),::inflection::dialog::SpeakableString(u"example"));
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"«example»", u"foobar"), ::inflection::dialog::SpeakableString(u"example", u"foobar"));
    locale = ::inflection::util::LocaleUtils::BRAZIL().getName().c_str();
    conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"“example”"), ::inflection::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteFr")
{
    auto locale = ::inflection::util::LocaleUtils::FRANCE().getName().c_str();
    auto conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"“example”"), ::inflection::dialog::SpeakableString(u"example"));
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"“example”", u"exampel"), ::inflection::dialog::SpeakableString(u"example", u"exampel"));
    locale = ::inflection::util::LocaleUtils::CANADIAN_FRENCH().getName().c_str();
    conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"“example”"), ::inflection::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteIt")
{
    auto locale = ::inflection::util::LocaleUtils::ITALIAN().getName().c_str();
    auto conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"«example»"), ::inflection::dialog::SpeakableString(u"example"));
    locale = ::inflection::util::LocaleUtils::SWITZERLAND_ITALIAN().getName().c_str();
    conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"«example»"), ::inflection::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testQuoteDe")
{
    auto locale = ::inflection::util::LocaleUtils::GERMANY().getName().c_str();
    auto conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"„example“"), ::inflection::dialog::SpeakableString(u"example"));
    locale = ::inflection::util::LocaleUtils::SWITZERLAND_GERMAN().getName().c_str();
    conceptFactory = getCommonConceptFactory_Quote(locale);
    runTestCase(conceptFactory, ::inflection::dialog::SpeakableString(u"«example»"), ::inflection::dialog::SpeakableString(u"example"));
}

TEST_CASE("QuoteTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    iccf_quote(nullptr, nullptr, &error);
    checkForFailure(&error);
}
