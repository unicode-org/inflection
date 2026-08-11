/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/Chunk.hpp>
#include <string>
#include <vector>

/**
 * @brief An arbitrary access linked-list chain of tokens.
 * @details This class enables access to a chain of @ref Token "Token"s as a doubly linked-list.
 * A TokenChain starts with a token that is a Token_Head ends with a Token_Tail.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::TokenChain
    : public inflection::tokenizer::Chunk
{
public:
    typedef Chunk super;

private:
    std::vector<Token *> tokenArray {  };
    int32_t headStartChar { };

public:
    /**
     * Returns the start of this TokenChain as a Token_Head.
     */
    virtual Token_Head* getHead() const;
    /**
     * Returns the end of this TokenChain as a Token_Tail.
     */
    virtual Token_Tail* getTail() const;
    /**
     * Returns true if both TokenChains are equal.
     *
     * @param rhs The other TokenChain.
     * @return true if both TokenChains are equal.
     */
    bool operator==(const inflection::tokenizer::TokenChain& rhs) const;
    /**
     * Returns a unique <code>std::size_t</code> hash of the entire TokenChain. This hash is calculated by summing all the
     * hashes of each Token in the TokenChain.
     *
     * The hash is calculated by successively multiplying the current hash by <code>int32_t(31)</code>, then adding the
     * next Token in the chain's Token::hashCode() to that.
     *
     * @return A unique hash of the TokenChain.
     */
    std::size_t operator()(const inflection::tokenizer::TokenChain& tokenChain) const noexcept;
    /**
     * Returns a sub-TokenChain of this TokenChain, given two offsets. This function will do its best to preserve
     * existing tokens within the chain (most likely only cutting off the last token if the end offset happens to be in
     * the middle of it).
     *
     * @param start The offset within this TokenChain to start the new TokenChain from.
     * @param end The offset within this TokenChain to end the new TokenChain on.
     * @return A sub-TokenChain based on the start and end offsets.
     */
    virtual TokenChain* getNewTokenChain(int32_t start, int32_t end) const;
    /**
     * Get the _value_ of the Token at the specified index. The _value_ is whatever is specified by the implementation
     * of the Token's <code>getValue()</code> function.
     *
     * @param index The index within the chain to retrieve the value from.
     * @return The Token's _value_ at the specified index.
     */
    virtual const ::std::u16string & getTokenValue(int32_t index) const;
    /**
     * Returns the Token from the specified boundary index in the original string.
     * Only the indexes provided by Token::getStartChar() and Token::getEndChar() in this TokenChain are valid.
     * All other index values will return nullptr.
     *
     * @return A pointer to the Token at specified boundary index in the original string.
     */
    virtual Token* getToken(int32_t index) const;
    /**
     * Returns a string representation of this TokenChain. This is **not** the TokenChain as a human readable string,
     * but rather an annotated representation of the Tokens within the chain.
     *
     * For example, <code>"What's Bill's phone number?"</code> will output something like:<br/>
     * <code>" | What's | What's | Bill's | phone | number | ?"</code><br/>
     * In which "What's" is repeated twice because they are both two different tokens: one has a _clean value_ of "what"
     * and the other "is".
     *
     * Additionally, all tokens that are not whitespace are separated by " | " in order to reduce excessive output and
     * emphasize token individuality.
     *
     * @return A <code>std::u16string</code> representing the contents of the TokenChain.
     */
    std::u16string toString() const override;
    /**
     * Returns a string that is a concatenation of all the Token values within this TokenChain.
     * This should be the equivalent of the original string.
     *
     * @return A <code>std::u16string</code> representing the contents of the TokenChain without annotation.
     */
    virtual std::u16string toOriginatingString() const;

public:
    /**
     * Constructs a new TokenChain, given a Token_Head and Token_Tail that eventually link to each-other, i.e is a
     * valid linked-list of Tokens.
     *
     * @param head The Token_Head that will become the start of this TokenChain.
     * @param tail the Token_Tail that will become the end of this TokenChain.
     */
    TokenChain(Token_Head* head, Token_Tail* tail);
    /**
     * Destructor
     */
    ~TokenChain() override;
};
