/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/tokenizer/TokenIterator.hpp>

/**
 * @brief A series of contiguous tokens of a string
 * @details This class is used to represent a group of Tokens. The Tokens can either be owned by
 * this Chunk or by something else, it can be assumed that if the Tokens are owned by this Chunk they
 * will be deleted when this Chunk is destroyed.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::Chunk
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

private: /* package */
    Token* start_ {  };
    Token* end_ {  };
    int32_t size {  };
    bool ownTokens {  };

public:
    /**
     * Returns the number of lexically significant words within the Chunk.
     *
     * @param start The index of the Token to start counting from (inclusive)
     * @param end The index of the Token to end counting on (inclusive)
     * @return The number of lexically significant words.
     */
    virtual int32_t getWordCount(int32_t start, int32_t end) const;
    /**
     * Returns the number of lexically significant words within the Chunk.
     *
     * @return The number of lexically significant words.
     */
    virtual int32_t getWordCount() const;
    /**
     * Returns the number of lexically significant words within the Chunk.
     *
     * @param chunk The Chunk to calculate the number of tokens on.
     * @return The number of lexically significant words.
     */
    static int32_t getWordCount(const Chunk* chunk);

private:
    static int32_t checkSize(const Token* start, const Token* end);

public:
    /**
     * Returns the number of Tokens within the Chunk.
     * @return The number of Tokens.
     */
    virtual int32_t getSize() const;
    /**
     * Returns the first Token within the Chunk.
     * @return The first Token within the Chunk.
     */
    virtual Token* getStart() const;
    /**
     * Returns the last Token within the Chunk.
     * @return The last Token within the Chunk.
     */
    virtual Token* getEnd() const;
    /**
     * Returns a Chunk encapsulating Tokens from start to end. The resulting Chunk does not own the Tokens that are
     * encapsulated, so it is important to not delete the original Chunk.
     *
     * @param start The index in which encapsulation begins.
     * @param end The index in which encapsulation ends.
     * @return A Chunk encapsulating Tokens from start to end.
     */
    virtual Chunk* getSubChunk(int32_t start, int32_t end) const;
    /**
     * Returns an iterator to the start of the Chunk.
     * @return Iterator to the start of the Chunk.
     */
    TokenIterator begin() const { return TokenIterator(this->start_, this->end_); }
    /**
     * Returns an iterator past the end of the Chunk.
     * @return Iterator past the end of the Chunk.
     */
    TokenIterator end() const { return TokenIterator(this->end_ != nullptr ? this->end_->getNext() : nullptr, this->end_); }
    /**
     * @return Returns a string representation of this Chunk.
     */
    virtual ::std::u16string getCleanValueString();
    /**
     * @return Returns a string representation of this Chunk with the option to normalize delimiters.
     */
    virtual ::std::u16string getCleanValueString(bool normalizeDelim);
    /**
     * @return Returns a normalized string representation of this Chunk with delimiters normalized.
     */
    virtual ::std::u16string getCleanString();
    /**
     * @return Returns a normalized string representation of this Chunk without delimiters normalized.
     */
    virtual ::std::u16string getString() const;

private:
    ::std::u16string getNormalizedString(bool normalizeDelim) const;

public:
    /**
     * @return Returns a normalized string representation of this Chunk without delimiters normalized.
     */
    ::std::u16string toString() const override;

    /**
     * Creates a Chunk from a linked-list of Tokens.
     *
     * @param start The first Token in the linked-list
     * @param end The last Token in the linked-list
     * @param adoptTokens Converts ownership of the Tokens to this Chunk. The Tokens will be deleted the this object is
     * deleted.
     */
    Chunk(Token* start, Token* end, bool adoptTokens = false);
    /**
     * Destructor
     */
    ~Chunk() override;

private:
    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    friend class TokenChain;
};
