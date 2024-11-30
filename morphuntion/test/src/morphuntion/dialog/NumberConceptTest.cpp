/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/LogToString.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>
#include <math.h>

TEST_CASE("NumberConceptTest#test_en_US_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"1" == one.getAsDigits().getPrint());
    REQUIRE(u"1st" == one.asDigits(u"ordinal"));
    REQUIRE(u"one" == one.getAsWords());
    REQUIRE(u"first" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#testBadVariants_en_US_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    LogToString stringLogger;

    REQUIRE(u"1st" == one.asDigits(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for en_US: [%digits-bad-variant]");

    stringLogger.logLines.clear();
    REQUIRE(u"one" == one.asWords(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for en_US: [%spellout-bad-variant]");
}

TEST_CASE("NumberConceptTest#testBadVariants_en_US_1_1")
{
    ::morphuntion::dialog::NumberConcept one(1.1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    LogToString stringLogger;

    REQUIRE(u"1st" == one.asDigits(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for en_US: [%digits-bad-variant]");

    stringLogger.logLines.clear();
    REQUIRE(u"one point one" == one.asWords(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for en_US: [%spellout-bad-variant]");
}

TEST_CASE("NumberConceptTest#test_en_US_2")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"2" == one.getAsDigits().getPrint());
    REQUIRE(u"2nd" == one.asDigits(u"ordinal"));
    REQUIRE(u"two" == one.getAsWords());
    REQUIRE(u"second" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_3")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)3, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"3" == one.getAsDigits().getPrint());
    REQUIRE(u"3rd" == one.asDigits(u"ordinal"));
    REQUIRE(u"three" == one.getAsWords());
    REQUIRE(u"third" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_4")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)4, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"4" == one.getAsDigits().getPrint());
    REQUIRE(u"4th" == one.asDigits(u"ordinal"));
    REQUIRE(u"four" == one.getAsWords());
    REQUIRE(u"fourth" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_109")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)109, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"109" == one.getAsDigits().getPrint());
    REQUIRE(u"109th" == one.asDigits(u"ordinal"));
    REQUIRE(u"one hundred nine" == one.getAsWords());
    REQUIRE(u"one hundred ninth" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_1090")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1090, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"1,090" == one.getAsDigits().getPrint());
    REQUIRE(u"1,090th" == one.asDigits(u"ordinal"));
    REQUIRE(u"one thousand ninety" == one.getAsWords());
    REQUIRE(u"one thousand ninetieth" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#testIntegerDigits")
{
    ::morphuntion::dialog::NumberConcept number((int64_t)2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> minIntegerDigits(npc(number.minIntegerDigits(3)));

    REQUIRE(u"002" == minIntegerDigits->getAsDigits().getPrint());
    REQUIRE(u"2nd" == minIntegerDigits->asDigits(u"ordinal"));
    REQUIRE(u"two" == minIntegerDigits->getAsWords());
    REQUIRE(u"second" == minIntegerDigits->asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_negative1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)-1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"-1" == one.getAsDigits().getPrint());
    REQUIRE(u"\u22121st" == one.asDigits(u"ordinal"));
    REQUIRE(u"minus one" == one.getAsWords());
    REQUIRE(u"minus first" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_en_US_Absolute")
{
    ::morphuntion::dialog::NumberConcept negativeOne((int64_t)-1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> absolute(npc(negativeOne.getAbsolute()));
    REQUIRE(u"1" == absolute->getAsDigits().getPrint());
    REQUIRE(u"1st" == absolute->asDigits(u"ordinal"));
    REQUIRE(u"one" == absolute->getAsWords());
    REQUIRE(u"first" == absolute->asWords(u"ordinal"));
    ::morphuntion::dialog::NumberConcept negativeDecimal(-10.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    absolute.reset(npc(negativeDecimal.getAbsolute()));
    REQUIRE(u"10.5" == absolute->getAsDigits().getPrint());
    REQUIRE(u"10th" == absolute->asDigits(u"ordinal"));
    REQUIRE(u"ten point five" == absolute->getAsWords());
    REQUIRE(u"10.5" == absolute->asWords(u"ordinal"));
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    absolute.reset(npc(one.getAbsolute()));
    REQUIRE(u"1" == absolute->getAsDigits().getPrint());
    REQUIRE(u"1st" == absolute->asDigits(u"ordinal"));
    REQUIRE(u"one" == absolute->getAsWords());
    REQUIRE(u"first" == absolute->asWords(u"ordinal"));
    ::morphuntion::dialog::NumberConcept decimal(10.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    absolute.reset(npc(decimal.getAbsolute()));
    REQUIRE(u"10.5" == absolute->getAsDigits().getPrint());
    REQUIRE(u"10th" == absolute->asDigits(u"ordinal"));
    REQUIRE(u"ten point five" == absolute->getAsWords());
    REQUIRE(u"10.5" == absolute->asWords(u"ordinal"));
    ::morphuntion::dialog::NumberConcept longNegativeDecimal(-10.123456789, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    absolute.reset(npc(longNegativeDecimal.getAbsolute()));
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> maxFractionDigits(npc(absolute->maxFractionDigits(9)));
    REQUIRE(u"10.123456789" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(longNegativeDecimal.maxFractionDigits(8)));
    absolute.reset(npc(maxFractionDigits->getAbsolute()));
    REQUIRE(u"10.12345679" == absolute->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(longNegativeDecimal.maxFractionDigits(7)));
    absolute.reset(npc(maxFractionDigits->getAbsolute()));
    REQUIRE(u"10.1234568" == absolute->getAsDigits().getPrint());
    absolute.reset(npc(longNegativeDecimal.getAbsolute()));
    maxFractionDigits.reset(npc(absolute->maxFractionDigits(6)));
    REQUIRE(u"10.123457" == maxFractionDigits->getAsDigits().getPrint());
}

TEST_CASE("NumberConceptTest#test_en_US_decimal")
{
    ::morphuntion::dialog::NumberConcept ten5(10.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"10.5" == ten5.getAsDigits().getPrint());
    REQUIRE(u"10th" == ten5.asDigits(u"ordinal"));
    REQUIRE(u"10th" == ten5.getAsOrdinalDigits());
    REQUIRE(u"ten point five" == ten5.getAsWords());
    REQUIRE(u"10.5" == ten5.asWords(u"ordinal"));
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> minFractionDigits(npc(ten5.minFractionDigits(2)));
    REQUIRE(u"10.50" == minFractionDigits->getAsDigits().getPrint());
    REQUIRE(u"ten point five" == minFractionDigits->getAsWords());
}

TEST_CASE("NumberConceptTest#test_en_US_longDecimal")
{
    ::morphuntion::dialog::NumberConcept one(10.123456789, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"10.123" == one.getAsDigits().getPrint());
    REQUIRE(u"ten point one two three" == one.getAsWords());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> maxFractionDigits(npc(one.maxFractionDigits(10)));
    REQUIRE(u"10.123456789" == maxFractionDigits->getAsDigits().getPrint());
    REQUIRE(u"ten point one two three four five six seven eight nine" == maxFractionDigits->getAsWords());
    maxFractionDigits.reset(npc(one.maxFractionDigits(9)));
    REQUIRE(u"10.123456789" == maxFractionDigits->getAsDigits().getPrint());
    REQUIRE(u"ten point one two three four five six seven eight nine" == maxFractionDigits->getAsWords());
    maxFractionDigits.reset(npc(one.maxFractionDigits(8)));
    REQUIRE(u"10.12345679" == maxFractionDigits->getAsDigits().getPrint());
    REQUIRE(u"ten point one two three four five six seven nine" == maxFractionDigits->getAsWords());
    maxFractionDigits.reset(npc(one.maxFractionDigits(7)));
    REQUIRE(u"10.1234568" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(6)));
    REQUIRE(u"10.123457" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(5)));
    REQUIRE(u"10.12346" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(4)));
    REQUIRE(u"10.1235" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(3)));
    REQUIRE(u"10.123" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(2)));
    REQUIRE(u"10.12" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(1)));
    REQUIRE(u"10.1" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(0)));
    REQUIRE(u"10" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(10)));
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> roundDown(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.123456789" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(9)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.123456789" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(8)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.12345678" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(7)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.1234567" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(6)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.123456" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(5)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.12345" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(4)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.1234" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(3)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.123" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(2)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.12" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(1)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10.1" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(0)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"10" == roundDown->getAsDigits().getPrint());
}

TEST_CASE("NumberConceptTest#test_en_US_longNumberDecimal")
{
    ::morphuntion::dialog::NumberConcept one(1.234567891234567E8, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(u"123,456,789.123" == one.getAsDigits().getPrint());
    REQUIRE(u"one hundred twenty-three million four hundred fifty-six thousand seven hundred eighty-nine point one two three" == one.getAsWords());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> maxFractionDigits(npc(one.maxFractionDigits(7)));
    // TODO BigDecimal doesn't exist
//    REQUIRE(u"123,456,789.1234567" == npc(one.maxFractionDigits(7))->getAsDigits().getPrint());
//    REQUIRE(u"one hundred twenty-three million four hundred fifty-six thousand seven hundred eighty-nine point one two three four five six seven" == maxFractionDigits->getAsWords());
    maxFractionDigits.reset(npc(one.maxFractionDigits(6)));
    REQUIRE(u"123,456,789.123457" == maxFractionDigits->getAsDigits().getPrint());
    REQUIRE(u"one hundred twenty-three million four hundred fifty-six thousand seven hundred eighty-nine point one two three four five seven" == maxFractionDigits->getAsWords());
    maxFractionDigits.reset(npc(one.maxFractionDigits(5)));
    REQUIRE(u"123,456,789.12346" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(4)));
    REQUIRE(u"123,456,789.1235" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(3)));
    REQUIRE(u"123,456,789.123" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(2)));
    REQUIRE(u"123,456,789.12" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(1)));
    REQUIRE(u"123,456,789.1" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(0)));
    REQUIRE(u"123,456,789" == maxFractionDigits->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(7)));
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> roundDown(npc(maxFractionDigits->roundDown()));
    // TODO BigDecimal doesn't exist
//    REQUIRE(u"123,456,789.1234567" == roundDown->getAsDigits().getPrint());
//    maxFractionDigits.reset(npc(one.maxFractionDigits(6)));
//    roundDown.reset(npc(maxFractionDigits->roundDown()));
//    REQUIRE(u"123,456,789.123456" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(5)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789.12345" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(4)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789.1234" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(3)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789.123" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(2)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789.12" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(1)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789.1" == roundDown->getAsDigits().getPrint());
    maxFractionDigits.reset(npc(one.maxFractionDigits(0)));
    roundDown.reset(npc(maxFractionDigits->roundDown()));
    REQUIRE(u"123,456,789" == roundDown->getAsDigits().getPrint());
}

TEST_CASE("NumberConceptTest#test_en_US_grouping")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1090, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> grouped(npc(one.grouping(true)));
    REQUIRE(u"1,090" == grouped->getAsDigits().getPrint());
    REQUIRE(u"1,090th" == grouped->asDigits(u"ordinal"));
    grouped.reset(npc(one.grouping(false)));
    REQUIRE(u"1090" == grouped->getAsDigits().getPrint());
    REQUIRE(u"1,090th" == grouped->asDigits(u"ordinal"));
}

TEST_CASE("NumberConceptTest#testLocaleMixture")
{
    ::morphuntion::dialog::NumberConcept one(10.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::GERMANY());
    REQUIRE(u"10,5" == one.getAsDigits().getPrint());
    REQUIRE(u"10.5" == one.getAsDigits().getSpeak());
    REQUIRE(u"10th" == one.asDigits(u"ordinal"));
    REQUIRE(u"ten point five" == one.getAsWords());
    REQUIRE(u"10.5" == one.asWords(u"ordinal"));
    REQUIRE(::morphuntion::util::LocaleUtils::US() == one.getLanguage());
    REQUIRE(::morphuntion::util::LocaleUtils::GERMANY() == one.getRegionLocale());
    REQUIRE(u"NumberConcept{10,5,en_US,de_DE}" == one.toString());
}

//TEST_CASE("NumberConceptTest#testLocaleMixtureWithRegionKeyword")
//{
//    morphuntion::util::ULocale usEnglishGermanRegion("en_US@rg=dezzzz");
//    ::morphuntion::dialog::NumberConcept one(10.5, morphuntion::util::LocaleUtils::US(), usEnglishGermanRegion);
//    REQUIRE(u"10,5" == one.getAsDigits().getPrint()); // German decimal point
//    REQUIRE(u"10.5" == one.getAsDigits().getSpeak());
//    REQUIRE(u"10th" == one.asDigits(u"ordinal"));
//    REQUIRE(u"ten point five" == one.getAsWords());
//    REQUIRE(u"10.5" == one.asWords(u"ordinal"));
//    REQUIRE(::morphuntion::util::LocaleUtils::US() == one.getLanguage());
//    REQUIRE(usEnglishGermanRegion == one.getRegionLocale());
//    REQUIRE(u"NumberConcept{10,5,en_US,en_US@rg=dezzzz}" == one.toString());
//}

TEST_CASE("NumberConceptTest#test_de_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN());
    REQUIRE(u"1" == one.getAsDigits().getPrint());
    REQUIRE(u"1." == one.asDigits(u"ordinal"));
    REQUIRE(u"eins" == one.getAsWords());
    REQUIRE(u"ein" == one.asWords(u"cardinal-masculine"));
    REQUIRE(u"eine" == one.asWords(u"cardinal-feminine"));
    REQUIRE(u"ein" == one.asWords(u"cardinal-neuter"));
    REQUIRE(u"erste" == one.asWords(u"ordinal"));
}

TEST_CASE("NumberConceptTest#test_fr_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH());
    REQUIRE(u"1" == one.getAsDigits().getPrint());
    REQUIRE(u"1er" == one.asDigits(u"ordinal"));
    REQUIRE(u"1er" == one.asDigits(u"ordinal-masculine"));
    REQUIRE(u"1re" == one.asDigits(u"ordinal-feminine"));
    REQUIRE(u"un" == one.getAsWords());
    REQUIRE(u"un" == one.asWords(u"cardinal-masculine"));
    REQUIRE(u"une" == one.asWords(u"cardinal-feminine"));
    REQUIRE(u"premier" == one.asWords(u"ordinal-masculine"));
    REQUIRE(u"première" == one.asWords(u"ordinal-feminine"));
}

TEST_CASE("NumberConceptTest#testBadVariants_fr_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH());
    LogToString stringLogger;

    REQUIRE(u"1er" == one.asDigits(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for fr: [%digits-bad-variant]");

    stringLogger.logLines.clear();
    REQUIRE(u"un" == one.asWords(u"bad-variant"));
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid variant for fr: [%spellout-bad-variant]");
}

TEST_CASE("NumberConceptTest#test_es_1")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH());
    REQUIRE(u"1" == one.getAsDigits().getPrint());
    REQUIRE(u"1.º" == one.asDigits(u"ordinal"));
    REQUIRE(u"1.ᵉʳ" == one.asDigits(u"ordinal-masculine-adjective"));
    REQUIRE(u"1.º" == one.asDigits(u"ordinal-masculine"));
    REQUIRE(u"1.ª" == one.asDigits(u"ordinal-feminine"));
    REQUIRE(u"uno" == one.getAsWords());
    REQUIRE(u"un" == one.asWords(u"cardinal-masculine"));
    REQUIRE(u"una" == one.asWords(u"cardinal-feminine"));
    REQUIRE(u"primero" == one.asWords(u"ordinal-masculine"));
    REQUIRE(u"primera" == one.asWords(u"ordinal-feminine"));
}

TEST_CASE("NumberConceptTest#testSwiss")
{
    REQUIRE(u"1’000" == ::morphuntion::dialog::NumberConcept((int64_t)1000, ::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN(), ::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN()).getAsDigits().getPrint());
    REQUIRE(u"1.000" == ::morphuntion::dialog::NumberConcept((int64_t)1000, ::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN(), ::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN()).getAsDigits().getSpeak());
    REQUIRE(u"1" == ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"1,1" == ::morphuntion::dialog::NumberConcept(1.1, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"80" == ::morphuntion::dialog::NumberConcept((int64_t)80, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"huitante" == ::morphuntion::dialog::NumberConcept((int64_t)80, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(), ::morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"1’000" == ::morphuntion::dialog::NumberConcept((int64_t)1000, ::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN(), ::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN()).getAsDigits().getPrint());
    REQUIRE(u"1.000" == ::morphuntion::dialog::NumberConcept((int64_t)1000, ::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN(), ::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN()).getAsDigits().getSpeak());
}

TEST_CASE("NumberConceptTest#testBelgium")
{
    REQUIRE(u"1" == ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"1,1" == ::morphuntion::dialog::NumberConcept(1.1, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"80" == ::morphuntion::dialog::NumberConcept((int64_t)80, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"quatre-vingts" == ::morphuntion::dialog::NumberConcept((int64_t)80, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"septante" == ::morphuntion::dialog::NumberConcept((int64_t)70, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"nonante" == ::morphuntion::dialog::NumberConcept((int64_t)90, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH(), ::morphuntion::util::LocaleUtils::BELGIUM_FRENCH()).getAsDigits().getSpeak());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).getAsDigits().getPrint());
    REQUIRE(u"59" == ::morphuntion::dialog::NumberConcept((int64_t)59, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).getAsDigits().getSpeak());
}

TEST_CASE("NumberConceptTest#testDanish")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::DANISH(), ::morphuntion::util::LocaleUtils::DANISH());
    REQUIRE(u"et" == one.getAsWords());
    REQUIRE(u"en" == one.asWords(u"cardinal-common"));
    REQUIRE(u"et" == one.asWords(u"cardinal-neuter"));
    ::morphuntion::dialog::NumberConcept two((int64_t)2, ::morphuntion::util::LocaleUtils::DANISH(), ::morphuntion::util::LocaleUtils::DANISH());
    REQUIRE(u"anden" == two.asWords(u"ordinal-common"));
    REQUIRE(u"andet" == two.asWords(u"ordinal-neuter"));
    ::morphuntion::dialog::NumberConcept eleven((int64_t)11, ::morphuntion::util::LocaleUtils::DANISH(), ::morphuntion::util::LocaleUtils::DANISH());
    REQUIRE(u"elleve" == eleven.asWords(u"cardinal-common"));
    REQUIRE(u"elleve" == eleven.asWords(u"cardinal-neuter"));
}

TEST_CASE("NumberConceptTest#testSwedish")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::SWEDISH(), ::morphuntion::util::LocaleUtils::SWEDISH());
    REQUIRE(u"ett" == one.getAsWords());
    REQUIRE(u"en" == one.asWords(u"cardinal-reale"));
    REQUIRE(u"ett" == one.asWords(u"cardinal-neuter"));
    REQUIRE(u"1:a" == one.asDigits(u"ordinal-reale"));
    REQUIRE(u"1:e" == one.asDigits(u"ordinal-masculine"));
    ::morphuntion::dialog::NumberConcept two((int64_t)2, ::morphuntion::util::LocaleUtils::SWEDISH(), ::morphuntion::util::LocaleUtils::SWEDISH());
    REQUIRE(u"två" == two.asWords(u"cardinal-neuter"));
    REQUIRE(u"andra" == two.asWords(u"ordinal-neuter"));
    REQUIRE(u"andre" == two.asWords(u"ordinal-masculine"));
    REQUIRE(u"2:a" == two.asDigits(u"ordinal-reale"));
    REQUIRE(u"2:e" == two.asDigits(u"ordinal-masculine"));
    ::morphuntion::dialog::NumberConcept eleven((int64_t)11, ::morphuntion::util::LocaleUtils::SWEDISH(), ::morphuntion::util::LocaleUtils::SWEDISH());
    REQUIRE(u"elfte" == eleven.asWords(u"ordinal-masculine"));
    REQUIRE(u"elva" == eleven.asWords(u"cardinal-neuter"));
    REQUIRE(u"11:e" == eleven.asDigits(u"ordinal-reale"));
    REQUIRE(u"11:e" == eleven.asDigits(u"ordinal-masculine"));
}

TEST_CASE("NumberConceptTest#testNorwegian")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::NORWEGIAN(), ::morphuntion::util::LocaleUtils::NORWEGIAN());
    REQUIRE(u"én" == one.getAsWords());
    REQUIRE(u"én" == one.asWords(u"cardinal-masculine"));
    REQUIRE(u"ei" == one.asWords(u"cardinal-feminine"));
    REQUIRE(u"ett" == one.asWords(u"cardinal-neuter"));
    ::morphuntion::dialog::NumberConcept two((int64_t)2, ::morphuntion::util::LocaleUtils::NORWEGIAN(), ::morphuntion::util::LocaleUtils::NORWEGIAN());
    REQUIRE(u"andre" == two.asWords(u"ordinal-masculine"));
    REQUIRE(u"andre" == two.asWords(u"ordinal-feminine"));
    REQUIRE(u"andre" == two.asWords(u"ordinal-neuter"));
    ::morphuntion::dialog::NumberConcept five((int64_t)5, ::morphuntion::util::LocaleUtils::NORWEGIAN(), ::morphuntion::util::LocaleUtils::NORWEGIAN());
    REQUIRE(u"femte" == five.asWords(u"ordinal-masculine"));
    REQUIRE(u"femte" == five.asWords(u"ordinal-feminine"));
    REQUIRE(u"femte" == five.asWords(u"ordinal-neuter"));
    REQUIRE(u"femte" == five.asWords(u"ordinal-plural"));
}

TEST_CASE("NumberConceptTest#testRussian")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::RUSSIAN(), ::morphuntion::util::LocaleUtils::RUSSIAN());
    REQUIRE(u"один" == one.getAsWords());
    REQUIRE(u"один" == one.asWords(u"cardinal-masculine"));
    REQUIRE(u"одна" == one.asWords(u"cardinal-feminine"));
    REQUIRE(u"одно" == one.asWords(u"cardinal-neuter"));
    REQUIRE(u"одни" == one.asWords(u"cardinal-plural"));
    REQUIRE(u"первый" == one.asWords(u"ordinal-masculine"));
    REQUIRE(u"первая" == one.asWords(u"ordinal-feminine"));
    REQUIRE(u"первое" == one.asWords(u"ordinal-neuter"));
    REQUIRE(u"первые" == one.asWords(u"ordinal-plural"));
}

TEST_CASE("NumberConceptTest#testPortuguese")
{
    ::morphuntion::dialog::NumberConcept twoThousand((int64_t)2002, ::morphuntion::util::LocaleUtils::BRAZIL(), ::morphuntion::util::LocaleUtils::BRAZIL());
    REQUIRE(u"dois mil e dois" == twoThousand.getAsWords());
    REQUIRE(u"dois mil e dois" == twoThousand.asWords(u"cardinal-masculine"));
    REQUIRE(u"duas mil e duas" == twoThousand.asWords(u"cardinal-feminine"));
}

TEST_CASE("NumberConceptTest#testAustrian")
{
    ::morphuntion::dialog::NumberConcept number((int64_t)1000, ::morphuntion::util::LocaleUtils::AUSTRIA(), ::morphuntion::util::LocaleUtils::AUSTRIA());
    INFO(::std::string("actual: ") + morphuntion::util::StringUtils::to_string(number.getAsDigits().getPrint()));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1 000", u"1.000") == number.getAsDigits());
}

TEST_CASE("NumberConceptTest#testHasFraction")
{
    REQUIRE(::morphuntion::dialog::NumberConcept(1.001f, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).hasFraction());
    REQUIRE(::morphuntion::dialog::NumberConcept(1.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).hasFraction());
    REQUIRE(!::morphuntion::dialog::NumberConcept(1.0, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).hasFraction());
    REQUIRE(!::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).hasFraction());
    REQUIRE(!::morphuntion::dialog::NumberConcept((int64_t)238, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).hasFraction());
}

TEST_CASE("NumberConceptTest#testRound")
{
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> maxFractionDigits;
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> rounded;

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(1.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"1" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(3.2, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"3" == rounded->getAsDigits().getPrint());

    rounded.reset(npc(::morphuntion::dialog::NumberConcept(3.8, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).roundFloor()));
    maxFractionDigits.reset(npc(rounded->maxFractionDigits(0)));
    REQUIRE(u"3" == maxFractionDigits->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.9, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.01, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundUp()));
    REQUIRE(u"5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.99, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundUp()));
    REQUIRE(u"5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(6.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfDown()));
    REQUIRE(u"6" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(6.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfUp()));
    REQUIRE(u"7" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(7.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfDown()));
    REQUIRE(u"7" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(7.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfUp()));
    REQUIRE(u"8" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(1.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"2" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(3.2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(3.8, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).maxFractionDigits(0)));
    rounded.reset(npc(rounded->roundCeiling()));
    REQUIRE(u"4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.9, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(4.2, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"2" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept((int64_t)4, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept((int64_t)0, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"0" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept((int64_t)-2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"-2" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-1.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"-2" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-3.2, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"-3" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-3.8, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"-3" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-4.9, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundCeiling()));
    REQUIRE(u"-4" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-4.2, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundFloor()));
    REQUIRE(u"-5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-4.01, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundUp()));
    REQUIRE(u"-5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-4.99, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundUp()));
    REQUIRE(u"-5" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-6.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfDown()));
    REQUIRE(u"-6" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-6.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfUp()));
    REQUIRE(u"-7" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-7.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfDown()));
    REQUIRE(u"-7" == rounded->getAsDigits().getPrint());

    maxFractionDigits.reset(npc(::morphuntion::dialog::NumberConcept(-7.5, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(0)));
    rounded.reset(npc(maxFractionDigits->roundHalfUp()));
    REQUIRE(u"-8" == rounded->getAsDigits().getPrint());
}

TEST_CASE("NumberConceptTest#testSpokenWords")
{
    ::morphuntion::dialog::NumberConcept one((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1", u"uno") == one.getAsSpokenWords());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1", u"un") == one.asSpokenWords(u"cardinal-masculine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1", u"una") == one.asSpokenWords(u"cardinal-feminine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1", u"primero") == one.asSpokenWords(u"ordinal-masculine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"1", u"primera") == one.asSpokenWords(u"ordinal-feminine"));
    ::morphuntion::dialog::NumberConcept ten((int64_t)10, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"10", u"diez") == ten.getAsSpokenWords());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"10", u"diez") == ten.asSpokenWords(u"cardinal-masculine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"10", u"diez") == ten.asSpokenWords(u"cardinal-feminine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"10", u"décimo") == ten.asSpokenWords(u"ordinal-masculine"));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"10", u"décima") == ten.asSpokenWords(u"ordinal-feminine"));
}

TEST_CASE("NumberConceptTest#testOrdinal")
{
    auto value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).asDigits(u"ordinal");
    REQUIRE(u"1st" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).asDigits(u"ordinal");
    REQUIRE(u"2nd" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)3, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).asDigits(u"ordinal");
    REQUIRE(u"3rd" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)4, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).asDigits(u"ordinal");
    REQUIRE(u"4th" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)109, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).asDigits(u"ordinal");
    REQUIRE(u"109th" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).asDigits(u"ordinal");
    REQUIRE(u"1." == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asDigits(u"ordinal");
    REQUIRE(u"1.º" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asDigits(u"ordinal-feminine");
    REQUIRE(u"1.ª" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asDigits(u"ordinal-masculine");
    REQUIRE(u"1.º" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::MEXICO_SPANISH(), ::morphuntion::util::LocaleUtils::MEXICO_SPANISH()).asDigits(u"ordinal");
    REQUIRE(u"1º." == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::MEXICO_SPANISH(), ::morphuntion::util::LocaleUtils::MEXICO_SPANISH()).asDigits(u"ordinal-feminine");
    REQUIRE(u"1ª." == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::MEXICO_SPANISH(), ::morphuntion::util::LocaleUtils::MEXICO_SPANISH()).asDigits(u"ordinal-masculine");
    REQUIRE(u"1º." == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).asDigits(u"ordinal");
    REQUIRE(u"1er" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).asDigits(u"ordinal-feminine");
    REQUIRE(u"1re" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).asDigits(u"ordinal-masculine");
    REQUIRE(u"1er" == value);
}

TEST_CASE("NumberConceptTest#testNumeral")
{
    auto value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).getAsWords();
    REQUIRE(u"one" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).getAsWords();
    REQUIRE(u"two" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)3, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).getAsWords();
    REQUIRE(u"three" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)4, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).getAsWords();
    REQUIRE(u"four" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)109, ::morphuntion::util::LocaleUtils::ENGLISH(), ::morphuntion::util::LocaleUtils::ENGLISH()).getAsWords();
    REQUIRE(u"one hundred nine" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).getAsWords();
    REQUIRE(u"eins" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).asWords(u"cardinal-feminine");
    REQUIRE(u"eine" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).asWords(u"cardinal-masculine");
    REQUIRE(u"ein" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).asWords(u"cardinal-neuter");
    REQUIRE(u"ein" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)16, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).getAsWords();
    REQUIRE(u"sechzehn" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)17, ::morphuntion::util::LocaleUtils::GERMAN(), ::morphuntion::util::LocaleUtils::GERMAN()).getAsWords();
    REQUIRE(u"siebzehn" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).getAsWords();
    REQUIRE(u"uno" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asWords(u"cardinal-feminine");
    REQUIRE(u"una" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asWords(u"cardinal-masculine");
    REQUIRE(u"un" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asWords(u"ordinal-feminine");
    REQUIRE(u"primera" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).asWords(u"ordinal-masculine");
    REQUIRE(u"primero" == value);
    value = ::morphuntion::dialog::NumberConcept(1.2, ::morphuntion::util::LocaleUtils::SPANISH(), ::morphuntion::util::LocaleUtils::SPANISH()).getAsWords();
    REQUIRE(u"uno coma dos" == value);
    value = ::morphuntion::dialog::NumberConcept(1.2, ::morphuntion::util::LocaleUtils::MEXICO_SPANISH(), ::morphuntion::util::LocaleUtils::MEXICO_SPANISH()).getAsWords();
    REQUIRE(u"uno punto dos" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).getAsWords();
    REQUIRE(u"un" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).asWords(u"cardinal-feminine");
    REQUIRE(u"une" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::FRENCH(), ::morphuntion::util::LocaleUtils::FRENCH()).asWords(u"cardinal-masculine");
    REQUIRE(u"un" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::CHINESE(), ::morphuntion::util::LocaleUtils::CHINESE()).asWords(u"cardinal");
    REQUIRE(u"二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::CHINESE(), ::morphuntion::util::LocaleUtils::CHINESE()).asWords(u"cardinal-alternate2");
    REQUIRE(u"两" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::CHINESE(), ::morphuntion::util::LocaleUtils::CHINESE()).asWords(u"cardinal");
    REQUIRE(u"二十二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::CHINESE(), ::morphuntion::util::LocaleUtils::CHINESE()).asWords(u"cardinal-alternate2");
    REQUIRE(u"二十二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::CHINESE(), ::morphuntion::util::LocaleUtils::CHINESE()).asWords(u"ordinal");
    REQUIRE(u"第二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE(), ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE()).asWords(u"cardinal");
    REQUIRE(u"二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE(), ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE()).asWords(u"cardinal-alternate2");
    REQUIRE(u"兩" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE(), ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE()).asWords(u"cardinal");
    REQUIRE(u"廿二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE(), ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE()).asWords(u"cardinal-alternate2");
    REQUIRE(u"廿二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::TAIWAN()).asWords(u"cardinal");
    REQUIRE(u"二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)2, ::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::TAIWAN()).asWords(u"cardinal-alternate2");
    REQUIRE(u"兩" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::TAIWAN()).asWords(u"cardinal");
    REQUIRE(u"二十二" == value);
    value = ::morphuntion::dialog::NumberConcept((int64_t)22, ::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::TAIWAN()).asWords(u"cardinal-alternate2");
    REQUIRE(u"二十二" == value);
}

static void assertMaxFractionalDigits(int32_t expected, double value, int32_t given)
{
    ::morphuntion::dialog::NumberConcept originalConcept(value, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> numConcept(npc(originalConcept.maxFractionDigitsAboveOne(given)));
    REQUIRE(numConcept->getMaximumFractionDigits() >= 0);
    REQUIRE(expected == numConcept->getMaximumFractionDigits());
}

static void assertMaxFractionalDigits(int32_t expected, double value)
{
    assertMaxFractionalDigits(expected, value, 2);
}

TEST_CASE("NumberConceptTest#testBasic")
{
    assertMaxFractionalDigits(2, 31415.926535);
    assertMaxFractionalDigits(2, 3141.5926535);
    assertMaxFractionalDigits(2, 314.15926535);
    assertMaxFractionalDigits(2, 31.415926535);
    assertMaxFractionalDigits(2, 3.1415926535);
    assertMaxFractionalDigits(2, 0.31415926535);
    assertMaxFractionalDigits(3, 0.031415926535);
    assertMaxFractionalDigits(4, 0.0031415926535);
    assertMaxFractionalDigits(5, 3.1415926535E-4);
    assertMaxFractionalDigits(6, 3.1415926535E-5);
    assertMaxFractionalDigits(7, 3.1415926535E-6);
    assertMaxFractionalDigits(8, 3.1415926535E-7);
    assertMaxFractionalDigits(9, 3.1415926535E-8);
    assertMaxFractionalDigits(10, 3.1415926535E-9);
    assertMaxFractionalDigits(11, 3.1415926535E-10);
}

TEST_CASE("NumberConceptTest#testNegative")
{
    assertMaxFractionalDigits(2, -31415.926535);
    assertMaxFractionalDigits(2, -3141.5926535);
    assertMaxFractionalDigits(2, -314.15926535);
    assertMaxFractionalDigits(2, -31.415926535);
    assertMaxFractionalDigits(2, -3.1415926535);
    assertMaxFractionalDigits(2, -0.31415926535);
    assertMaxFractionalDigits(3, -0.031415926535);
    assertMaxFractionalDigits(4, -0.0031415926535);
    assertMaxFractionalDigits(5, -3.1415926535E-4);
    assertMaxFractionalDigits(6, -3.1415926535E-5);
    assertMaxFractionalDigits(7, -3.1415926535E-6);
    assertMaxFractionalDigits(8, -3.1415926535E-7);
    assertMaxFractionalDigits(9, -3.1415926535E-8);
    assertMaxFractionalDigits(10, -3.1415926535E-9);
    assertMaxFractionalDigits(11, -3.1415926535E-10);
}

TEST_CASE("NumberConceptTest#testZero")
{
    int32_t given = 2;
    double value = 0.0;
    assertMaxFractionalDigits(given, value, given);
    given = 10;
    assertMaxFractionalDigits(given, value, given);
    given = 20;
    assertMaxFractionalDigits(given, value, given);
}

static constexpr double EFFECTIVE_ZERO_THRESHOLD { 1.0E-16 };

TEST_CASE("NumberConceptTest#testEffectiveZero")
{
    double value = EFFECTIVE_ZERO_THRESHOLD - EFFECTIVE_ZERO_THRESHOLD / 10.0;
    int32_t given = 2;
    assertMaxFractionalDigits(given, value, given);
    assertMaxFractionalDigits(given, -value, given);
    given = 10;
    assertMaxFractionalDigits(given, value, given);
    assertMaxFractionalDigits(given, -value, given);
    given = 20;
    assertMaxFractionalDigits(given, value, given);
    assertMaxFractionalDigits(given, -value, given);
}

// TODO Add this back some day when the API is supported.
//TEST_CASE("NumberConceptTest#testRoundedValue")
//{
//    REQUIRE((int64_t)0 == npc(npc(::morphuntion::dialog::NumberConcept((int64_t)0, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)0 == npc(npc(::morphuntion::dialog::NumberConcept(0.001, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)1 == npc(npc(::morphuntion::dialog::NumberConcept(0.99999, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)1 == npc(npc(::morphuntion::dialog::NumberConcept((int64_t)1, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)1 == npc(npc(::morphuntion::dialog::NumberConcept(1.0, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)1 == npc(npc(::morphuntion::dialog::NumberConcept(1.00001, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE((int64_t)1 == npc(npc(npc(::morphuntion::dialog::NumberConcept(1.005, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->roundDown())->getRoundedValue())->getValue());
//    REQUIRE(1.01 == npc(npc(npc(::morphuntion::dialog::NumberConcept(1.005, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->roundUp())->getRoundedValue())->getValue());
//    REQUIRE(1.01 == npc(npc(::morphuntion::dialog::NumberConcept(1.01, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE(INT64_MAX == npc(npc(::morphuntion::dialog::NumberConcept(INT64_MAX, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//    REQUIRE(DBL_MAX == npc(npc(::morphuntion::dialog::NumberConcept(DBL_MAX, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()).maxFractionDigits(2))->getRoundedValue())->getValue());
//}

TEST_CASE("NumberConceptTest#testRoundingModeSpellout")
{
    ::morphuntion::dialog::NumberConcept numberConcept(1.234556678, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    ::std::unique_ptr<::morphuntion::dialog::NumberConcept> rounded;

    REQUIRE(u"one point two three five" == numberConcept.getAsWords());
    rounded.reset(npc(numberConcept.roundDown()));
    REQUIRE(u"one point two three four" == rounded->getAsWords());
    ::morphuntion::dialog::NumberConcept numberConcept2(4.3, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    rounded.reset(npc(numberConcept2.roundDown()));
    REQUIRE(u"4.3" == rounded->getAsDigits().getPrint());
    REQUIRE(u"four point three" == numberConcept2.getAsWords());
    rounded.reset(npc(numberConcept2.roundDown()));
    REQUIRE(u"four point three" == rounded->getAsWords());
}

TEST_CASE("NumberConceptTest#testUnrealNumbers")
{
    ::morphuntion::dialog::NumberConcept numberConcept(INFINITY, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"∞") == numberConcept.getAsDigits());
    REQUIRE(u"infinity" == numberConcept.getAsWords());
    ::morphuntion::dialog::NumberConcept numberConcept2(NAN, ::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"NaN") == numberConcept2.getAsDigits());
    REQUIRE(u"not a number" == numberConcept2.getAsWords());
}
