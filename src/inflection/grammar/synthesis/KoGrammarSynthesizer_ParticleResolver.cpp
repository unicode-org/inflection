/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleResolver.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <memory>
#include <unicode/uchar.h>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

KoGrammarSynthesizer_ParticleResolver::KoGrammarSynthesizer_ParticleResolver(const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle)
    : dictionary(*npc(inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::KOREA())))
    , vowelSet(vowelSet)
    , englishTokenizer(npc(inflection::tokenizer::TokenizerFactory::createTokenizer(inflection::util::LocaleUtils::ENGLISH())))
    , vowelParticle(vowelParticle)
    , consonantParticle(consonantParticle)
    , vowelsWithRieul(&vowelSet == &KoGrammarSynthesizer::VOWELS_WITH_RIEUL())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&rieulProperty, {inflection::dictionary::PhraseProperties::RIEUL_END_TAG()}));
}

KoGrammarSynthesizer_ParticleResolver::KoGrammarSynthesizer_ParticleResolver(const KoGrammarSynthesizer_ParticleResolver& other) noexcept
    : dictionary(other.dictionary)
    , vowelSet(other.vowelSet)
    , englishTokenizer(npc(inflection::tokenizer::TokenizerFactory::createTokenizer(inflection::util::LocaleUtils::ENGLISH())))
    , vowelParticle(other.vowelParticle)
    , consonantParticle(other.consonantParticle)
    , vowelsWithRieul(other.vowelsWithRieul)
{
}

KoGrammarSynthesizer_ParticleResolver::~KoGrammarSynthesizer_ParticleResolver()
{
}

std::optional<std::u16string> KoGrammarSynthesizer_ParticleResolver::switchParticleValue(const ::std::u16string& str, bool enableInflectionGuess) const
{
    std::u16string result(str);
    if (result.ends_with(consonantParticle)) {
        result.resize(result.length() - consonantParticle.size());
    }
    else if (!vowelParticle.empty() && result.ends_with(vowelParticle)) {
        result.resize(result.length() - vowelParticle.size());
    }

    auto particle(getParticleValue(result, enableInflectionGuess));
    if (particle.empty()) {
        return {};
    }
    return result.append(particle);
}

static void removeEnclosingPunctuation(std::u16string_view& str) {
    int32_t strLength = static_cast<int32_t>(str.length());
    char32_t codePoint = 0;
    int32_t idx = 0;
    int32_t startPunctIdx = 0;
    while (idx < strLength) {
        codePoint = inflection::util::StringViewUtils::codePointAt(str, idx);
        if ((U_GET_GC_MASK(static_cast<UChar32>(codePoint)) & (U_GC_PS_MASK)) != 0) {
            break;
        }
        idx += U16_LENGTH(codePoint);
    }
    if (idx < strLength) {
        startPunctIdx = idx;
        idx += U16_LENGTH(codePoint);
    }
    while (idx < strLength) {
        codePoint = inflection::util::StringViewUtils::codePointAt(str, idx);
        if ((U_GET_GC_MASK(static_cast<UChar32>(codePoint)) & (U_GC_PS_MASK|U_GC_PE_MASK)) != 0) {
            break;
        }
        idx += U16_LENGTH(codePoint);
    }
    if (idx < strLength) {
        str.remove_suffix(strLength - startPunctIdx);
    }
}

std::u16string KoGrammarSynthesizer_ParticleResolver::getRelevantString(std::u16string_view str) const
{
    ::std::u16string_view strToTokenize(str);
    removeEnclosingPunctuation(strToTokenize);
    if (!strToTokenize.empty() && !static_cast<bool>(uscript_hasScript(
            static_cast<UChar32>(inflection::util::StringViewUtils::codePointAt(strToTokenize, int32_t(strToTokenize.length() - 1))),
            USCRIPT_HANGUL)))
    {
        std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(npc(englishTokenizer->createTokenChain(std::u16string(strToTokenize))));
        for (const inflection::tokenizer::Token *token = tokenChain->getTail(); token != nullptr; token = token->getPrevious()) {
            const auto &value = npc(token)->getValue();
            if (!value.empty() && static_cast<bool>(u_isalnum(static_cast<UChar32>(inflection::util::StringViewUtils::codePointAt(value, int32_t(value.length() - 1)))))) {
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
            if (!enableInflectionGuess && (relevantString.empty() || !bool(u_isalpha((UChar32)inflection::util::StringViewUtils::codePointAt(relevantString, int32_t(relevantString.length() - 1)))))) {
                return {};
            }
            if (::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(relevantString, ::inflection::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), ::inflection::lang::StringFilterUtil::LATIN_SCRIPT(), {})) {
                std::u16string lowercase;
                inflection::util::StringViewUtils::lowercase(&lowercase, relevantString, inflection::util::LocaleUtils::KOREAN());
                if ((vowelsWithRieul && isEndsWithRieul(lowercase))
                    || ::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREAN(), lowercase))
                {
                    return vowelParticle;
                }
            }
            else if (::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(relevantString, ::inflection::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), vowelSet, {})) {
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
    ::inflection::util::UnicodeSetUtils::removeSetFromString(&withoutPunctuation, ::inflection::dictionary::PhraseProperties::PUNCTUATION(), str);
    if (dictionary.hasAllProperties(withoutPunctuation, rieulProperty)) {
        return true;
    }
    return ::inflection::dictionary::PhraseProperties::isEndsWithUnicodeSets(str, ::inflection::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), KoGrammarSynthesizer::DEFAULT_RIEUL_END(), {});
}

} // namespace inflection::grammar::synthesis
