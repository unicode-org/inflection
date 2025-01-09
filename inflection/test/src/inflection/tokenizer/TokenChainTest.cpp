/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/tokenizer/TokenUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>
#include "inflection/util/LogToString.hpp"
#include <memory>
#include <set>

TEST_CASE("TokenChainTest#testTokenChain") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> newTokenChain(tokenChain->getNewTokenChain(9, npc(tokenChain->getEnd())->getEndChar()));
    REQUIRE(npc(npc(tokenChain->getEnd())->getPrevious())->getValue() == npc(npc(newTokenChain->getEnd())->getPrevious())->getValue());

    newTokenChain.reset(tokenChain->getNewTokenChain(0, npc(tokenChain->getEnd())->getEndChar() - 1));
    REQUIRE(npc(npc(tokenChain->getStart())->getNext())->getValue() == npc(npc(newTokenChain->getStart())->getNext())->getValue());
    REQUIRE(npc(tokenChain->getStart())->isHead());
    REQUIRE_FALSE(npc(tokenChain->getStart())->isTail());
    REQUIRE_FALSE(npc(tokenChain->getEnd())->isHead());
    REQUIRE(npc(tokenChain->getEnd())->isTail());
}

TEST_CASE("TokenChainTest#withStartCharNonZero")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    ::std::unique_ptr<::inflection::tokenizer::Chunk> subChunk(tokenChain->getSubChunk(9, 16));
    REQUIRE((subChunk->toString() == u"how are"));
}

TEST_CASE("TokenChainTest#testTokenChainEmpty") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u""));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> newTokenChain(tokenChain->getNewTokenChain(0, npc(tokenChain->getEnd())->getEndChar()));
    REQUIRE(newTokenChain->getHead() == npc(newTokenChain->getEnd())->getPrevious());
}

TEST_CASE("TokenChainTest#testBounds") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"Test"));
    REQUIRE(tokenChain->getHead() == tokenChain->getToken(-1));
    LogToString stringLogger;
    REQUIRE(nullptr == tokenChain->getToken(5));
    REQUIRE(stringLogger.logLines.size() == 1);
    INFO(inflection::util::StringUtils::to_string(stringLogger.logLines[0]));
    CHECK(stringLogger.logLines[0] == u"[ERROR] Attempting to getToken with invalid index: 5");
}

TEST_CASE("TokenChainTest#testTokenChainEquality") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> secondChain(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    REQUIRE(*tokenChain == *secondChain);
    REQUIRE(*tokenChain == *tokenChain);
}

TEST_CASE("TokenChainTest#testTokenChainInequality") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> nonMatching(tokenizer->createTokenChain(u"Hey Siri how are you. I like to use \"quoted tokens\"!"));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> otherNonMatching(tokenizer->createTokenChain(u"Hey Siri how is the weather today."));
    REQUIRE(*tokenChain != *nonMatching);
    REQUIRE_FALSE(*tokenChain == *nonMatching);
    REQUIRE(*tokenChain != *otherNonMatching);
    REQUIRE_FALSE(*tokenChain == *otherNonMatching);
}

TEST_CASE("TokenChainTest#testEmptyChainRelationships") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> emptyChain(tokenizer->createTokenChain(u""));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> otherEmpty(tokenizer->createTokenChain(u""));
    REQUIRE(*emptyChain == *otherEmpty);
    REQUIRE_FALSE(*emptyChain != *otherEmpty);
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> spaceChain(tokenizer->createTokenChain(u" "));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> periodChain(tokenizer->createTokenChain(u"."));
    REQUIRE_FALSE(*emptyChain == *spaceChain);
    REQUIRE_FALSE(*emptyChain == *periodChain);
}

TEST_CASE("TokenChainTest#testToString")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tk(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    tk->setStyle(inflection::tokenizer::Tokenizer::DEFAULT);
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChain(tk->createTokenChain(u"A test?"));
    ::std::vector<::std::u16string> expected({
        u"Head{begin=0, end=0, value='', cleanValue=''} insignificant",
        u"Word{begin=0, end=1, value='A', cleanValue='a'} significant",
        u"Delim{begin=1, end=2, value=' ', cleanValue=' '} insignificant",
        u"Word{begin=2, end=6, value='test', cleanValue='test'} significant",
        u"Word{begin=6, end=7, value='?', cleanValue='?'} insignificant",
        u"Tail{begin=7, end=7, value='', cleanValue=''} insignificant",
    });
    REQUIRE(tokChain->getSize() == (int32_t)expected.size());
    auto expectedItr = expected.begin();
    for (const auto &token : *tokChain) {
        REQUIRE(*expectedItr == token.toString() + (token.isSignificant() ? u" significant" : u" insignificant"));
        ++expectedItr;
    }

    ::std::u16string input = u"translate  hello there";
    tokChain.reset(tk->createTokenChain(input));
    REQUIRE(input == tokChain->toOriginatingString());
    input = u"playing  with core-foundation  library";
    tokChain.reset(tk->createTokenChain(input));
    REQUIRE(input == tokChain->toOriginatingString());
    input = u"What's Bill's phone number?";
    tokChain.reset(tk->createTokenChain(input));
    REQUIRE(input == tokChain->toOriginatingString());
    REQUIRE(u"Tokens:  | What's | Bill's | phone | number | ?" == tokChain->toString());
}

TEST_CASE("TokenChainTest#hashTest")
{
    std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer(inflection::tokenizer::TokenizerFactory::createTokenizer(inflection::util::LocaleUtils::US()));
    std::unique_ptr<inflection::tokenizer::TokenChain> chain1(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));
    std::unique_ptr<inflection::tokenizer::TokenChain> chain2(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!"));

    std::set<inflection::tokenizer::TokenChain*> s;
    s.insert(chain1.get());
    s.insert(chain2.get());

    auto it = s.begin();

    REQUIRE(*chain2 == **it);
    REQUIRE(*chain1 == **(++it));
}