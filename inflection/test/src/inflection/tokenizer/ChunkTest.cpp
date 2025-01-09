/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include "inflection/tokenizer/Tokenizer.hpp"
#include "inflection/tokenizer/TokenizerFactory.hpp"
#include "inflection/tokenizer/Token_Head.hpp"
#include "inflection/tokenizer/TokenChain.hpp"
#include "inflection/util/LocaleUtils.hpp"
#include "inflection/npc.hpp"
#include <memory>

TEST_CASE("ChunkTest#testWordCount") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US())));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));

    REQUIRE(tokenChain->getWordCount() == 11);
    REQUIRE(tokenChain->getWordCount(22, 51) == 6);

    ::std::unique_ptr<::inflection::tokenizer::Chunk> chunk(tokenChain->getSubChunk(22, 51));
    REQUIRE(chunk->getWordCount() == 6);
}

TEST_CASE("ChunkTest#testNull") /* throws(Exception) */
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US())));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain1(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain2(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));

    CHECK_THROWS(inflection::tokenizer::Chunk(tokenChain1->getStart(), nullptr));
    CHECK_THROWS(inflection::tokenizer::Chunk(nullptr, tokenChain2->getEnd()));
    CHECK_THROWS(inflection::tokenizer::Chunk(tokenChain1->getStart(), tokenChain2->getEnd()));
}
