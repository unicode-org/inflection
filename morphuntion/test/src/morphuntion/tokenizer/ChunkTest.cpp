/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include "morphuntion/tokenizer/Tokenizer.hpp"
#include "morphuntion/tokenizer/TokenizerFactory.hpp"
#include "morphuntion/tokenizer/Token_Head.hpp"
#include "morphuntion/tokenizer/TokenChain.hpp"
#include "morphuntion/util/LocaleUtils.hpp"
#include "morphuntion/npc.hpp"
#include <memory>

TEST_CASE("ChunkTest#testWordCount") /* throws(Exception) */
{
    ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer(npc(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::US())));
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));

    REQUIRE(tokenChain->getWordCount() == 11);
    REQUIRE(tokenChain->getWordCount(22, 51) == 6);

    ::std::unique_ptr<::morphuntion::tokenizer::Chunk> chunk(tokenChain->getSubChunk(22, 51));
    REQUIRE(chunk->getWordCount() == 6);
}

TEST_CASE("ChunkTest#testNull") /* throws(Exception) */
{
    ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer(npc(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::US())));
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain1(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain2(npc(tokenizer->createTokenChain(u"hey siri how are you. I like to use \"quoted tokens\"!")));

    CHECK_THROWS(morphuntion::tokenizer::Chunk(tokenChain1->getStart(), nullptr));
    CHECK_THROWS(morphuntion::tokenizer::Chunk(nullptr, tokenChain2->getEnd()));
    CHECK_THROWS(morphuntion::tokenizer::Chunk(tokenChain1->getStart(), tokenChain2->getEnd()));
}
