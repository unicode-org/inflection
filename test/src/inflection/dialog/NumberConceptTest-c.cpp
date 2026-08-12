/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/NumberConcept.h>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include "util/StringContainer.hpp"
#include <memory>
#include <math.h>

typedef ::inflection::util::AutoCRelease<IDNumberConcept*, inum_destroy> AutoNumberConceptRelease;
typedef ::inflection::util::AutoCRelease<IDSpeakableString*, iss_destroy> AutoSpeakableStringRelease;

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
    REQUIRE(U_SUCCESS(error));

    AutoSpeakableStringRelease digits(inum_asDigitsCopy(one.value, &error));
    REQUIRE(U_SUCCESS(error));
    compareStringsNumberConceptTest(u"1", ((::inflection::dialog::SpeakableString *) digits.value)->getPrint());

    auto ordinal = u"ordinal";
    char16_t oridinalDigitsVar[4] = {};
    inum_asOrginalDigitsVariant(one.value, ordinal, oridinalDigitsVar, std::size(oridinalDigitsVar), &error);
    REQUIRE(U_SUCCESS(error));
    compareStringsNumberConceptTest(u"1st", oridinalDigitsVar);

    util::StringContainer<IDNumberConcept, inum_asWords> words(one.value);
    REQUIRE(words);
    compareStringsNumberConceptTest(u"one", words.value);

    char16_t wordsVar[6] = {};
    inum_asWordsVariant(one.value, ordinal, wordsVar, std::size(wordsVar), &error);
    REQUIRE(U_SUCCESS(error));
    compareStringsNumberConceptTest(u"first", wordsVar);
}

TEST_CASE("NumberConceptTest-c#testSpokenWords")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::SPANISH().getName().c_str();
    AutoNumberConceptRelease one(inum_createFromInt64((int64_t)1, locale, locale, &error));
    REQUIRE(U_SUCCESS(error));

    AutoSpeakableStringRelease spokenWords(inum_asSpokenWordsCopy(one.value, &error));
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"uno"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    auto cardinalMasculine = u"cardinal-masculine";
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, cardinalMasculine, &error);
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"un"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    auto cardinalFeminine = u"cardinal-feminine";
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, cardinalFeminine, &error);
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"una"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    auto ordinalMasculine = u"ordinal-masculine";
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, ordinalMasculine, &error);
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"primero"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

    auto ordinalFeminine = u"ordinal-feminine";
    spokenWords = inum_asSpokenWordsVariantCopy(one.value, ordinalFeminine, &error);
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"1", u"primera"), *((::inflection::dialog::SpeakableString*) spokenWords.value));

}

TEST_CASE("NumberConceptTest-c#testLocaleMixture")
{
    auto error = U_ZERO_ERROR;
    auto usLocale= ::inflection::util::LocaleUtils::US().getName().c_str();
    auto deLocale= ::inflection::util::LocaleUtils::GERMANY().getName().c_str();
    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, usLocale, deLocale, &error));
    REQUIRE(U_SUCCESS(error));

    AutoSpeakableStringRelease digits(inum_asDigitsCopy(ten5.value, &error));
    REQUIRE(U_SUCCESS(error));
    runSpokenWordsTest(::inflection::dialog::SpeakableString(u"10,5", u"10.5"), *((::inflection::dialog::SpeakableString*) digits.value));

    util::StringContainer<IDNumberConcept, inum_toString> oneString(ten5.value);
    REQUIRE(oneString);
    compareStringsNumberConceptTest(u"NumberConcept{10,5,en_US,de_DE}", oneString.value);
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimal")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();
    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, locale, locale, &error));
    REQUIRE(U_SUCCESS(error));

    util::StringContainer<IDNumberConcept, inum_asOrdinalDigits> ordinalDigits(ten5.value);
    REQUIRE(ordinalDigits);
    compareStringsNumberConceptTest(u"10th", ordinalDigits.value);

    util::StringContainer<IDNumberConcept, inum_asWords> words(ten5.value);
    REQUIRE(words);
    compareStringsNumberConceptTest(u"ten point five" , words.value);
}

TEST_CASE("NumberConceptTest-c#test_en_US_decimalClone")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::US().getName().c_str();

    AutoNumberConceptRelease ten5(inum_createFromDouble(10.5, locale, locale, &error));
    REQUIRE(U_SUCCESS(error));

    AutoNumberConceptRelease cloneten5(inum_clone(ten5.value, &error));
    REQUIRE(U_SUCCESS(error));

    util::StringContainer<IDNumberConcept, inum_asOrdinalDigits> ordinalDigits(ten5.value);
    REQUIRE(ordinalDigits);
    compareStringsNumberConceptTest(u"10th", ordinalDigits.value);

    util::StringContainer<IDNumberConcept, inum_asOrdinalDigits> cloneOrdinalDigits(cloneten5.value);
    REQUIRE(cloneOrdinalDigits);
    compareStringsNumberConceptTest(u"10th", cloneOrdinalDigits.value);
}