/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>

TEST_CASE("PhrasePropertiesTest#testVowels")
{
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"apple"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"APPLE"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"ａｐｐｌｅ"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"apples"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"Kelly"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"Yahoo!"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u""));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"!@,."));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"1"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"é"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"résumé"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"RÉSUMÉ"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"fluß"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"가"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"용인"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"apple"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"APPLE"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"umbrella"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"unicorn"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"banana"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"yellow"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"\"Apple\""));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"\"Banana\""));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u""));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"!@,."));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"1"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"é"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"É"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), u"han"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"an"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"han"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"ban"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"hôtel"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"HomePod"));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), u"œsophagien"));

    // C++ is quite content to construct a C++ string from a nullptr that will fail at runtime. This is an issue with the language and not the code being tested.
    //CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), nullptr));
    //CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), nullptr));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"빌리"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Billy"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"아이패드"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"iPad"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"블루투스"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Bluetooth"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"에릭"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Eric"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"애플"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Apple"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Richard'"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"CJ"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"CGV"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"GKL"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"GS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KT"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"LG"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"LS"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"NHN"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"NICE"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"OCI"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SKT"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SK"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"S-oil"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SBS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KBS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"MBC"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"C&C"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SKC"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"STX"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"WISCOM"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"Samsung"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SJM"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"NPC"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KCC"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KT&G"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"ITX"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"E&M"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"CNH"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"CS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"DMS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"EG"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"HRS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"ENT."));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KCI"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KHM"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"KTH"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SDN"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"SGA"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"METALLOY"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"TPC"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"YTN"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"TV"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"DVS"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"OPC"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREA(), u"ICT"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"zombie"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"Messier"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"Spotify"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"now"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"YK"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"Watch"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"BMW"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"Vodafone"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"IBM"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"ATM"));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"Stockholm"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::TURKISH(), u"PDF"));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::DUTCH(), u"bakkerij"));
}

TEST_CASE("PhrasePropertiesTest#testUnicodeSets")
{
    ::icu4cxx::UnicodeSet IMPORTANT_SET(u"[[[:Letter:][:Decimal_Number:]]-[hH]]");
    ::icu4cxx::UnicodeSet A_LETTER(u"[aA]");
    ::icu4cxx::UnicodeSet B_LETTER(u"[bB]");
    ::icu4cxx::UnicodeSet C_LETTER(u"[cC]");
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hab", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"habc", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"Z", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hZa", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"haZb", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"habZc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ba", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"bah", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abc", IMPORTANT_SET, B_LETTER, {&C_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abh", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abch", IMPORTANT_SET, B_LETTER, {&C_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"Z", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZh", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZbh", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZbch", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
}
