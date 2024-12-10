/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_ParticleResolver.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Token_Tail.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

KoGrammarSynthesizer_ParticleResolver::KoGrammarSynthesizer_ParticleResolver(const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle)
    : dictionary(*npc(morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::KOREA())))
    , vowelSet(vowelSet)
    , englishTokenizer(morphuntion::tokenizer::TokenizerFactory::createTokenizer(morphuntion::util::LocaleUtils::ENGLISH()))
    , vowelParticle(vowelParticle)
    , consonantParticle(consonantParticle)
    , vowelsWithRieul(&vowelSet == &KoGrammarSynthesizer::VOWELS_WITH_RIEUL())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&rieulProperty, {morphuntion::dictionary::PhraseProperties::RIEUL_END_TAG()}));
}

KoGrammarSynthesizer_ParticleResolver::KoGrammarSynthesizer_ParticleResolver(const KoGrammarSynthesizer_ParticleResolver& other) noexcept
    : dictionary(other.dictionary)
    , vowelSet(other.vowelSet)
    , englishTokenizer(morphuntion::tokenizer::TokenizerFactory::createTokenizer(morphuntion::util::LocaleUtils::ENGLISH()))
    , vowelParticle(other.vowelParticle)
    , consonantParticle(other.consonantParticle)
    , vowelsWithRieul(other.vowelsWithRieul)
{
}

KoGrammarSynthesizer_ParticleResolver::~KoGrammarSynthesizer_ParticleResolver()
{
    delete englishTokenizer;
}

std::optional<std::u16string> KoGrammarSynthesizer_ParticleResolver::switchParticleValue(const ::std::u16string& str, bool enableInflectionGuess) const
{
    std::u16string result(str);
    if (morphuntion::util::StringViewUtils::endsWith(result, consonantParticle)) {
        result.resize(result.length() - consonantParticle.size());
    }
    else if (!vowelParticle.empty() && morphuntion::util::StringViewUtils::endsWith(result, vowelParticle)) {
        result.resize(result.length() - vowelParticle.size());
    }

    auto particle(getParticleValue(result, enableInflectionGuess));
    if (particle.empty()) {
        return {};
    }
    return result.append(particle);
}

std::u16string KoGrammarSynthesizer_ParticleResolver::getRelevantString(std::u16string_view str) const
{
    ::std::u16string_view strToTokenize(str);
    if (morphuntion::util::UnicodeSetUtils::containsSome(openParenthesesSet, strToTokenize)) {
        icu4cxx::RegularExpression localParenthesesMatcher(parenthesesMatcher);
        localParenthesesMatcher.setText(strToTokenize);
        if (localParenthesesMatcher.findNext()) {
            auto suffixLen = strToTokenize.length() - localParenthesesMatcher.start(0);
            if (suffixLen > 0) {
                strToTokenize.remove_suffix(suffixLen);
            }
        }
    }
    if (!strToTokenize.empty() && !morphuntion::lang::StringFilterUtil::HANGUL_SCRIPT().contains((UChar32)morphuntion::util::StringViewUtils::codePointAt(strToTokenize, int32_t(strToTokenize.length() - 1)))) {
        // We don't care about the strings in the phonetic Hangul script. Return the relevant last word to check without punctuation.
        std::unique_ptr<morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(englishTokenizer)->createTokenChain(std::u16string(strToTokenize))));
        for (const morphuntion::tokenizer::Token *token = tokenChain->getTail(); token != nullptr; token = token->getPrevious()) {
            const auto &value = npc(token)->getValue();
            if (!value.empty() && bool(u_isalnum((UChar32)morphuntion::util::StringViewUtils::codePointAt(value, int32_t(value.length() - 1))))) {
                // Get the relevant word for testing, which ends in an alphanumeric word.
                strToTokenize = strToTokenize.substr(npc(token)->getStartChar(), npc(token)->getLength());
                break;
            }
        }
    }
    return std::u16string(strToTokenize);
}

std::u16string KoGrammarSynthesizer_ParticleResolver::getParticleValue(std::u16string_view str, bool enableInflectionGuess) const
{
    if (!str.empty()) {
        if (!vowelSet.isEmpty()) {
            std::u16string relevantString(getRelevantString(str));
            if (!enableInflectionGuess && (relevantString.empty() || !bool(u_isalpha((UChar32)morphuntion::util::StringViewUtils::codePointAt(relevantString, int32_t(relevantString.length() - 1)))))) {
                return {};
            }
            if (::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(relevantString, ::morphuntion::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), ::morphuntion::lang::StringFilterUtil::LATIN_SCRIPT(), {})) {
                std::u16string lowercase;
                morphuntion::util::StringViewUtils::lowercase(&lowercase, relevantString, morphuntion::util::LocaleUtils::KOREAN());
                if ((vowelsWithRieul && isEndsWithRieul(lowercase))
                    || ::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREAN(), lowercase))
                {
                    return vowelParticle;
                }
            }
            else if (::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(relevantString, ::morphuntion::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), vowelSet, {})) {
                return vowelParticle;
            }
        }
        return consonantParticle;
    }
    return {};
}

bool KoGrammarSynthesizer_ParticleResolver::isEndsWithRieul(const ::std::u16string& str) const
{
    if (str.empty()) {
        return false;
    }
    ::std::u16string withoutPunctuation;
    ::morphuntion::util::UnicodeSetUtils::removeSetFromString(&withoutPunctuation, ::morphuntion::dictionary::PhraseProperties::PUNCTUATION(), str);
    if (dictionary.hasAllProperties(withoutPunctuation, rieulProperty)) {
        return true;
    }
    return ::morphuntion::dictionary::PhraseProperties::isEndsWithUnicodeSets(str, ::morphuntion::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), KoGrammarSynthesizer::DEFAULT_RIEUL_END(), {});
}

} // namespace morphuntion::grammar::synthesis
