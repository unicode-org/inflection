/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>

TEST_CASE("PhrasePropertiesTest#testVowels")
{
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"apple"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"APPLE"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"ａｐｐｌｅ"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"apples"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"Kelly"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"Yahoo!"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u""));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"!@,."));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"1"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"é"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"résumé"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"RÉSUMÉ"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"fluß"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"가"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"용인"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"apple"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"APPLE"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"umbrella"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"unicorn"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"banana"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"yellow"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"\"Apple\""));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"\"Banana\""));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u""));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"!@,."));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"1"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"é"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"É"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), u"han"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"an"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"han"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"ban"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"hôtel"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"HomePod"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), u"œsophagien"));

    // C++ is quite content to construct a C++ string from a nullptr that will fail at runtime. This is an issue with the language and not the code being tested.
    //CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), nullptr));
    //CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), nullptr));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"빌리"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Billy"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"아이패드"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"iPad"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"블루투스"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Bluetooth"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"에릭"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Eric"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"애플"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Apple"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Richard'"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"CJ"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"CGV"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"GKL"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"GS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KT"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"LG"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"LS"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"NHN"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"NICE"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"OCI"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SKT"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SK"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"S-oil"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SBS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KBS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"MBC"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"C&C"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SKC"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"STX"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"WISCOM"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"Samsung"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SJM"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"NPC"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KCC"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KT&G"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"ITX"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"E&M"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"CNH"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"CS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"DMS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"EG"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"HRS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"ENT."));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KCI"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KHM"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"KTH"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SDN"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"SGA"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"METALLOY"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"TPC"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"YTN"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"TV"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"DVS"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"OPC"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREA(), u"ICT"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"zombie"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"Messier"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"Spotify"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"now"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"YK"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"Watch"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"BMW"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"Vodafone"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"IBM"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"ATM"));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"Stockholm"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::TURKISH(), u"PDF"));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::DUTCH(), u"bakkerij"));
}

TEST_CASE("PhrasePropertiesTest#testUnicodeSets")
{
    ::icu4cxx::UnicodeSet IMPORTANT_SET(u"[[[:Letter:][:Decimal_Number:]]-[hH]]");
    ::icu4cxx::UnicodeSet A_LETTER(u"[aA]");
    ::icu4cxx::UnicodeSet B_LETTER(u"[bB]");
    ::icu4cxx::UnicodeSet C_LETTER(u"[cC]");
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hab", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"habc", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"Z", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"hZa", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"haZb", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"habZc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ha", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ab", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abc", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ba", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"bah", IMPORTANT_SET, A_LETTER, {}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abc", IMPORTANT_SET, B_LETTER, {&C_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abh", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"abch", IMPORTANT_SET, B_LETTER, {&C_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"Z", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZh", IMPORTANT_SET, A_LETTER, {}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZbh", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"aZbch", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(u"a", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {&B_LETTER}));
    CHECK_FALSE(::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(u"ah", IMPORTANT_SET, A_LETTER, {&B_LETTER, &C_LETTER}));
}
