/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/NumberConcept.h>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/LogToString.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include "util/CFUtils.hpp"
#include <memory>
#include <math.h>

typedef ::inflection::util::AutoCRelease<IDNumberConcept*, inum_destroy> AutoNumberConceptRelease;
typedef ::inflection::util::AutoCRelease<IDSpeakableString*, iss_destroy> AutoSpeakableStringRelease;

static void checkForSuccess(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_SUCCESS(*status));
}

static void compareStringsNumberConceptTest(::std::u16string_view expected, ::std::u16string_view actual) {
    INFO(::std::string("expected=") + ::inflection::util::StringViewUtils::to_string(expected) + " actual=" + ::inflection::util::StringViewUtils::to_string(expected));
    CHECK(expected == actual);
}

static void compareSpeakableStringsNumberConceptTest(const IDSpeakableString* expected, const IDSpeakableString* actual) {
    auto expectedDesciptiveString = ::inflection::util::StringViewUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::inflection::util::StringViewUtils::to_string(npc((const ::inflection::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::inflection::dialog::SpeakableString*) expected) == *((const ::inflection::dialog::SpeakableString*) actual));
}

static void runSpokenWordsTest(const ::inflection::dialog::SpeakableString &expected, const ::inflection::dialog::SpeakableString &actual) {
    compareSpeakableStringsNumberConceptTest((IDSpeakableString*) &expected,(IDSpeakableString*) &actual);
}

TEST_CASE("NumberConceptTest-c#test_en_US_1")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();

    AutoNumberConceptRelease one(inum_createFromInt64((int64_t) 1, locale, locale, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease digits(inum_asDigitsCopy(one.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"1", ((::inflection::dialog::SpeakableString *) digits.value)->getPrint());

    inflection::util::AutoCFRelease<CFStringRef> ordinal(util::TypeConversionUtils::to_CFString(u"ordinal"));
    inflection::util::AutoCFRelease<CFStringRef> oridinalDigitsVar(inum_asOrginalDigitsVariantCopy(one.value, ordinal.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"1st", ::util::TypeConversionUtils::to_u16string(oridinalDigitsVar.value));

    inflection::util::AutoCFRelease<CFStringRef> words(inum_asWordsCopy(one.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"one", ::util::TypeConversionUtils::to_u16string(words.value));

    inflection::util::AutoCFRelease<CFStringRef> wordsVar(inum_asWordsVariantCopy(one.value, ordinal.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"first", ::util::TypeConversionUtils::to_u16string(wordsVar.value));
}

TEST_CASE("NumberConceptTest-c#testSpokenWords")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    AutoNumberConceptRelease one(inum_createFromInt64((int64_t)1, locale, locale, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease spokenWords(inum_asSpokenWordsCopy(one.value, &error));
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"uno"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    inflection::util::AutoCFRelease<CFStringRef> cardinalMasculine(util::TypeConversionUtils::to_CFString(u"cardinal-masculine"));
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, cardinalMasculine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"un"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    inflection::util::AutoCFRelease<CFStringRef> cardinalFeminine(util::TypeConversionUtils::to_CFString(u"cardinal-feminine"));
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, cardinalFeminine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"una"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    inflection::util::AutoCFRelease<CFStringRef> ordinalMasculine(util::TypeConversionUtils::to_CFString(u"ordinal-masculine"));
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, ordinalMasculine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"primero"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    inflection::util::AutoCFRelease<CFStringRef> ordinalFeminine(util::TypeConversionUtils::to_CFString(u"ordinal-feminine"));
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, ordinalFeminine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"primera"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

}

TEST_CASE("NumberConceptTest-c#testLocaleMixture")
{
    auto error = U_ZERO_ERROR;
    auto usLocale= ::inflection::util::LocaleUtils::US().getName().c_str();
    auto deLocale= ::inflection::util::LocaleUtils::GERMANY().getName().c_str();
    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, usLocale, deLocale, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease digits(inum_asDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"10,5", u"10.5"), *((::inflection::dialog::SpeakableString*) digits.value));

    inflection::util::AutoCFRelease<CFStringRef> oneString(inum_toStringCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"NumberConcept{10,5,en_US,de_DE}", ::util::TypeConversionUtils::to_u16string(oneString.value));
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimal")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();
    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, locale, locale, &error));
    checkForSuccess(&error);

    inflection::util::AutoCFRelease<CFStringRef> ordinalDigits(inum_asOrdinalDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::TypeConversionUtils::to_u16string(ordinalDigits.value));

    inflection::util::AutoCFRelease<CFStringRef> words(inum_asWordsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"ten point five" , ::util::TypeConversionUtils::to_u16string(words.value));
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimalClone")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();

    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, locale, locale, &error));
    checkForSuccess(&error);

    AutoNumberConceptRelease cloneten5(inum_clone(ten5.value, &error));
    checkForSuccess(&error);

    inflection::util::AutoCFRelease<CFStringRef> ordinalDigits(inum_asOrdinalDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::TypeConversionUtils::to_u16string(ordinalDigits.value));

    inflection::util::AutoCFRelease<CFStringRef> cloneOrdinalDigits(inum_asOrdinalDigitsCopy(cloneten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::TypeConversionUtils::to_u16string(cloneOrdinalDigits.value));
}