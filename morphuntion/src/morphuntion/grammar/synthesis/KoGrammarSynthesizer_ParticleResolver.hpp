/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <optional>

class morphuntion::grammar::synthesis::KoGrammarSynthesizer_ParticleResolver final
{
private:
    const morphuntion::dictionary::DictionaryMetaData& dictionary;
    const icu4cxx::UnicodeSet& vowelSet;
    icu4cxx::UnicodeSet openParenthesesSet { u"[:Line_Break=Open_Punctuation:]" };
    const icu4cxx::RegularExpression parenthesesMatcher { u"[:Line_Break=Open_Punctuation:].*[[:Line_Break=Close_Punctuation:][:Line_Break=Close_Parenthesis:]]", 0, nullptr };
    morphuntion::tokenizer::Tokenizer* englishTokenizer {  };
    ::std::u16string vowelParticle {  };
    ::std::u16string consonantParticle {  };
    int64_t rieulProperty {  };
    bool vowelsWithRieul {  };

public:
    std::optional<std::u16string> switchParticleValue(const ::std::u16string& str, bool enableInflectionGuess) const;
private:
    std::u16string getRelevantString(std::u16string_view str) const;
public:
    std::u16string getParticleValue(std::u16string_view str, bool enableInflectionGuess) const;
private:
    bool isEndsWithRieul(const ::std::u16string& str) const;

public:
    KoGrammarSynthesizer_ParticleResolver(const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle);
    KoGrammarSynthesizer_ParticleResolver(const KoGrammarSynthesizer_ParticleResolver& other) noexcept;
    ~KoGrammarSynthesizer_ParticleResolver();
};
