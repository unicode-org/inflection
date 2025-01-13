/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/Object.hpp>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <string_view>

/**
 * @brief Provides an interface to turn a string into a TokenChain.
 * @details
 * **Overview**
 * Tokenization is the method that turns any given string of text into words (free morphemes).
 * For a language like English, the vast majority of words are easily separable by spaces and punctuation.
 * For the Chinese and Japanese languages, there normally are no whitespace or punctuation characters used to separate words.
 * So it becomes very difficult to find the word boundaries. Tokenization provides the boundaries of these words.
 *
 * **Languages with heavily compounded words**
 * Two or more nouns can be combined to make new compound words. Sometimes other morphemes that contain other
 * parts of speech properties, like adjectives, can be a part of compound word. In English, the components of
 * a compound word are often, but not always, separated by white spaces, e.g. "shopping list", "blackbird", and
 * "freeze-dried". By contrast, German and Dutch compound words are mostly written together as a single orthographic unit,
 * for example, German "Einkaufszettel" or Dutch "boodschappenlijst" (shopping list). Such compounds have to be decompounded
 * to retrieve the meaning of the individual components of the compound. German "Einkaufszettel" (shopping list)
 * can be decompounded as <Einkauf-s-zettel>. Decompounding would allow you to search for a list called "Einkauf".
 * The letter "s" is a fugenelement, which is not a part of either component of the compound word.
 *
 * It is also not desirable to over decompound words. For example, a German person may want to update their "Facebookstatus".
 * The proper decompounding is "Facebook" and "status"; however, "Facebook" is also a compound word, but by decompounding
 * it further, you change the meaning of the words when interpreting the decompounded words independently.
 * This is in contrast to a word like "blackbird" where the meaning of black and bird retain their meaning
 * when decompounded, but each component retains a more generic meaning of the original word instead of a completely meaning.
 * This behavior is helpful for relaxed searches.
 *
 * **Concepts related to tokenization**
 * Tokenization normally does not do morphological analysis to stem or lemmatize words. If such functionality is desired
 * Please use the Analyzer API instead.
 *
 * @see inflection::analysis::ConfigurableAnalyzer
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::Tokenizer
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

public:
    enum TokenizationType {
        /**
         * This is the default tokenization style. This includes parsing of Siri specific explicit tokens,
         * which have key-value pairs, and splitting runs of digits from letters.
         */
        DEFAULT,
        /**
         * Excludes all tokenization modifications.
         */
        UNMODIFIED
    };

private:
    typedef void (*modifyTokens)(const ::inflection::tokenizer::Tokenizer* tokenizer, std::u16string_view originalInput, inflection::tokenizer::Token_Head* head);
    TokenizationType tokenizationType { TokenizationType::DEFAULT };

private: /* package */
    const ::inflection::tokenizer::TokenExtractor* tokenExtractor {  };

public:
    /**
     * Creates a TokenChain from a string.
     */
    virtual TokenChain* createTokenChain(std::u16string_view charSequence) const;

public:

    /**
     * Destructor
     */
    ~Tokenizer() override;
    /**
     * Sets the style of tokenization.
     */
    virtual void setStyle(TokenizationType type);

protected: /* protected */
    explicit Tokenizer(const ::inflection::tokenizer::TokenExtractor* wordExtractor);
public:
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;

private:
    friend class TokenizerFactory;
};
