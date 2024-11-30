/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/NumberConcept.h>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/LogToString.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include "util/CFUtils.hpp"
#include <memory>
#include <math.h>

typedef ::morphuntion::util::AutoCRelease<MDNumberConcept*, mnum_destroy> AutoNumberConceptRelease;
typedef ::morphuntion::util::AutoCRelease<MDSpeakableString*, mss_destroy> AutoSpeakableStringRelease;

static void checkForSuccess(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error == nullptr);
}

static void compareStringsNumberConceptTest(::std::u16string_view expected, ::std::u16string_view actual) {
    INFO(::std::string("expected=") + ::morphuntion::util::StringViewUtils::to_string(expected) + " actual=" + ::morphuntion::util::StringViewUtils::to_string(expected));
    CHECK(expected == actual);
}

static void compareSpeakableStringsNumberConceptTest(const MDSpeakableString* expected, const MDSpeakableString* actual) {
    auto expectedDesciptiveString = ::morphuntion::util::StringViewUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) expected)->toString());
    auto actualDesciptiveString = ::morphuntion::util::StringViewUtils::to_string(npc((const ::morphuntion::dialog::SpeakableString*) actual)->toString());

    INFO(::std::string("expected=") + expectedDesciptiveString + " actual=" + actualDesciptiveString);
    CHECK(*((const ::morphuntion::dialog::SpeakableString*) expected) == *((const ::morphuntion::dialog::SpeakableString*) actual));
}

static void runSpokenWordsTest(const ::morphuntion::dialog::SpeakableString &expected, const ::morphuntion::dialog::SpeakableString &actual) {
    compareSpeakableStringsNumberConceptTest((MDSpeakableString*) &expected,(MDSpeakableString*) &actual);
}

TEST_CASE("NumberConceptTest-c#test_en_US_1")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));

    AutoNumberConceptRelease one(mnum_createFromInt64((int64_t) 1, cfLocale.value, cfLocale.value, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease digits(mnum_asDigitsCopy(one.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"1", ((::morphuntion::dialog::SpeakableString *) digits.value)->getPrint());

    morphuntion::util::AutoCFRelease<CFStringRef> ordinal(util::CFUtils::to_CFString(u"ordinal"));
    morphuntion::util::AutoCFRelease<CFStringRef> oridinalDigitsVar(mnum_asOrginalDigitsVariantCopy(one.value, ordinal.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"1st", ::util::CFUtils::to_u16string(oridinalDigitsVar.value));

    morphuntion::util::AutoCFRelease<CFStringRef> words(mnum_asWordsCopy(one.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"one", ::util::CFUtils::to_u16string(words.value));

    morphuntion::util::AutoCFRelease<CFStringRef> wordsVar(mnum_asWordsVariantCopy(one.value, ordinal.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"first", ::util::CFUtils::to_u16string(wordsVar.value));
}

TEST_CASE("NumberConceptTest-c#testSpokenWords")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::SPANISH()));
    AutoNumberConceptRelease one(mnum_createFromInt64((int64_t)1, cfLocale.value, cfLocale.value, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease spokenWords(mnum_asSpokenWordsCopy(one.value, &error));
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"1", u"uno"), *((::morphuntion::dialog::SpeakableString*) spokenWords.value));

    morphuntion::util::AutoCFRelease<CFStringRef> cardinalMasculine(util::CFUtils::to_CFString(u"cardinal-masculine"));
    spokenWords = mnum_asSpokenWordsVariantCopy(one.value, cardinalMasculine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"1", u"un"), *((::morphuntion::dialog::SpeakableString*) spokenWords.value));

    morphuntion::util::AutoCFRelease<CFStringRef> cardinalFeminine(util::CFUtils::to_CFString(u"cardinal-feminine"));
    spokenWords = mnum_asSpokenWordsVariantCopy(one.value, cardinalFeminine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"1", u"una"), *((::morphuntion::dialog::SpeakableString*) spokenWords.value));

    morphuntion::util::AutoCFRelease<CFStringRef> ordinalMasculine(util::CFUtils::to_CFString(u"ordinal-masculine"));
    spokenWords = mnum_asSpokenWordsVariantCopy(one.value, ordinalMasculine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"1", u"primero"), *((::morphuntion::dialog::SpeakableString*) spokenWords.value));

    morphuntion::util::AutoCFRelease<CFStringRef> ordinalFeminine(util::CFUtils::to_CFString(u"ordinal-feminine"));
    spokenWords = mnum_asSpokenWordsVariantCopy(one.value, ordinalFeminine.value, &error);
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"1", u"primera"), *((::morphuntion::dialog::SpeakableString*) spokenWords.value));

}

TEST_CASE("NumberConceptTest-c#testLocaleMixture")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> usLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));
    morphuntion::util::AutoCFRelease<CFLocaleRef> deLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::GERMANY()));
    AutoNumberConceptRelease ten5(mnum_createFromDouble(10.5, usLocale.value, deLocale.value, &error));
    checkForSuccess(&error);

    AutoSpeakableStringRelease digits(mnum_asDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    runSpokenWordsTest(::morphuntion::dialog::SpeakableString(u"10,5", u"10.5"), *((::morphuntion::dialog::SpeakableString*) digits.value));

    morphuntion::util::AutoCFRelease<CFStringRef> oneString(mnum_toStringCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"NumberConcept{10,5,en_US,de_DE}", ::util::CFUtils::to_u16string(oneString.value));
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimal")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));
    AutoNumberConceptRelease ten5(mnum_createFromDouble(10.5, cfLocale.value, cfLocale.value, &error));
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> ordinalDigits(mnum_asOrdinalDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::CFUtils::to_u16string(ordinalDigits.value));

    morphuntion::util::AutoCFRelease<CFStringRef> words(mnum_asWordsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"ten point five" , ::util::CFUtils::to_u16string(words.value));
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimalClone")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> cfLocale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::US()));

    AutoNumberConceptRelease ten5(mnum_createFromDouble(10.5, cfLocale.value, cfLocale.value, &error));
    checkForSuccess(&error);

    AutoNumberConceptRelease cloneten5(mnum_clone(ten5.value, &error));
    checkForSuccess(&error);

    morphuntion::util::AutoCFRelease<CFStringRef> ordinalDigits(mnum_asOrdinalDigitsCopy(ten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::CFUtils::to_u16string(ordinalDigits.value));

    morphuntion::util::AutoCFRelease<CFStringRef> cloneOrdinalDigits(mnum_asOrdinalDigitsCopy(cloneten5.value, &error));
    checkForSuccess(&error);
    compareStringsNumberConceptTest(u"10th", ::util::CFUtils::to_u16string(cloneOrdinalDigits.value));
}