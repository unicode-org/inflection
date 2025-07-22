/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/CharRange.hpp>
#include <string_view>

/**
 * @brief Provides representation of a lexical token.
 * @details Tokens provide word, morpheme, and delineation representation. A Token object
 * also stores various _attributes_, such as if the token is _whitespace_ or is
 * compositionally _significant_. It can also provide its _value_, its _clean value_,
 * and its _start_ and _stop_ characters.
 *
 * The Token object provides the foundational blocks of the main aspects of Inflection's functionalities, including
 * tokenization and grammar synthesis.
 *
 * <h3> Start and end character </h3>
 * Each Token object keeps track of where the Token is located within the <code>std::16string</code> represented by the
 * encompassing TokenChain. This is stored as the start and end character position. And can be retrieved by
 * getStartChar() and getEndChar() respectively.
 *
 * <h3> Value </h3>
 * This represents the *raw value* of the Token, without word or character normalization or any other filtering
 * performed by Inflection. This is retrievable using the getValue() function.
 *
 * <h3> Clean value </h3>
 * This represents the value of the Token after normalization, lemmatization, decompounding, or any other filtering
 * performed by Inflection. This is retrievable using the getCleanValue() function.
 *
 * <h3> Use within TokenChain </h3>
 * @ref Token "Token"s make up the building blocks of @ref TokenChain "TokenChain"s, in which an input stream
 * represented by a TokenChain is made up of an **arbitrary access doubly-linked-list** of @ref Token "Token"s.
 *
 * A Token is also somewhat contextually aware of its position within a TokenChain, and has methods in order to
 * determine if it is at the start or end of the chain. You can use getHead() and getTail() in order to detect if the
 * the Token is located at the beginning or end of the encompassing TokenChain.
 *
 * The way Token is implemented here, is that it is a node within a linked-list, which allows you to use Token
 * independently of a TokenChain. Simply use getNext() to advance to the next Token in the chain.
 *
 * That being said, it is recommended to use TokenIterator in C++ code, as it provides C++ iterator-like semantics.<br/>
 *
 * <h3> Other tokens </h3>
 * This class is meant to be overridden with classes that have more specific lexical meaning. Inflection provides
 * these classes as more precise inheritors:
 *  - Token_Head
 *  - Token_Tail
 *  - Token_Word
 *  - Token_Delim
 *  - Token_Explicit
 *  - Token_Group
 *  - Token_OverSizedToken
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::Token
    : public virtual CharRange
{
public:
    typedef ::inflection::tokenizer::CharRange super;

private:
    int32_t startChar {  };
    int32_t endChar {  };
    ::std::u16string value {  };
    ::std::u16string cleanValue {  };
    bool significant {  };
    bool whitespace {  };
    Token* previous {  };
    Token* next {  };

protected: /* package */
    /**
     * Creates a new Token object at the specified offset. The exact definition of this token is implementation-specific
     * and must be overridden.
     *
     * @return A Token* at the specified offset.
     * @param offset The offset in the TokenChain in which this token is located (i.e. it's index).
     */
    virtual Token* newInstance(int32_t offset) const = 0;

public:
    /**
     * Checks if the two Tokens are the same.
     *
     * @param rhs   Other Token being compared to.
     * @return      True if the two are the same, false otherwise.
     */
    bool operator==(const ::inflection::tokenizer::Token& rhs) const;
    /**
     * Generates a unique hash of this Token.
     *
     * The hash is calculated by casting the <code>std::hash</code> of the underlying value (returned by getValue()).
     *
     * @return A unique hash of this Token.
     */
    std::size_t operator()(const inflection::tokenizer::Token& token) const noexcept;

private:
    bool calculateWhiteSpace() const;

public:
    /**
     * Destructor
     */
    ~Token() override;
    /**
     * Returns the starting position of this Token within the underlying <code>std::u16string</code>
     * in terms of UTF-16 code units.
     */
    int32_t getStartChar() const override;
    /**
     * Returns the ending position of this Token within the underlying <code>std::u16string</code>
     * in terms of UTF-16 code units.
     */
    int32_t getEndChar() const override;
    /**
     * Returns the original length of this Token in terms of UTF-16 code units.
     */
    int32_t getLength() const;
    /**
     * Returns the original value of this Token.
     */
    const ::std::u16string& getValue() const;
    /**
     * Returns the clean value of this Token. Any type of normalization or modification of the value will be this clean value.
     * For example, this may be the lowercase value.
     */
    const ::std::u16string& getCleanValue() const;
    /**
     * Returns a pointer to the previous Token within the encompassing TokenChain.
     */
    Token* getPrevious() const;

protected: /* package */
    void setPrevious(Token* previous);

public:
    /**
     * Returns a pointer to the next Token within the encompassing TokenChain.
     */
    Token* getNext() const;

protected: /* package */
    void setNext(Token* next);

public:
    /**
     * Returns true when this token is _significant_ for interpretation. Insignificant tokens can include tokens with punctuation, like the hyphen.
     *
     * @return True if the token is _significant_. False otherwise.
     */
    bool isSignificant() const;
    /**
     * Returns true when this token only contains _whitespace_.
     *
     * @return True if the token is _whitespace_. False otherwise.
     */
    bool isWhitespace() const;
    /**
     * Generates a string representation of this Token for debugging purposes.
     * @return The string representation of this Token.
     */
    ::std::u16string toString() const override;
    /**
     * @return True if this Token is the first Token in the encompassing TokenChain. False otherwise.
     */
    bool isHead() const;
    /**
     * @return True if this Token is the last Token in the encompassing TokenChain. False otherwise.
     */
    bool isTail() const;

protected: /* package */
    Token(int32_t startChar, int32_t endChar, std::u16string_view value, std::u16string_view cleanValue, bool significant);
    Token(const Token&) = delete;
    Token& operator=(const Token&) = delete;

private:
    friend class TokenChain;
    friend class Token_Head;
    friend class Token_Tail;
    friend class Token_Delim;
    friend class Token_Word;
    friend class ::inflection::tokenizer::TokenUtil;
};
