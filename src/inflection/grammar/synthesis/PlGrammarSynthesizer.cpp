/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/PlGrammarSynthesizer.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/PlGrammarSynthesizer_PlDisplayFunction.hpp>
#include <inflection/tokenizer/Token.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <span>

namespace inflection::grammar::synthesis {

static constexpr struct {
    const char16_t* preposition;
    const char16_t* prefixLetters;
    const char16_t* prefixes[2];
    const char16_t* words[3];
} PREPOSITION_MAP[] = {
    {u"bez", u"", {}, {u"mnie"}},
    {u"nad", u"", {}, {u"mną", u"mnie", u"wszystko"}},
    {u"od", u"", {}, {u"mnie"}},
    {u"pod", u"", {}, {u"mną", u"mnie"}},
    {u"przed", u"", {}, {u"mną", u"mnie", u"wszystkim"}},
    {u"przez", u"", {}, {u"mnie"}},
    {u"w", u"fw", {}, {u"mnie"}},
    {u"z", u"zźśs", {u"ws", u"wz"}, {u"mną", u"dwoje"}},
};

static bool matchesPrefix(std::u16string_view word, std::span<const char16_t* const> prefixes)
{
    for (const auto& prefix : prefixes) {
        if (prefix == nullptr) {
            break;
        }
        if (word.starts_with(prefix)) {
            return true;
        }
    }
    return false;
}

static bool matchesWord(std::u16string_view word, std::span<const char16_t* const> words)
{
    for (const auto& wordToMatch : words) {
        if (wordToMatch == nullptr) {
            break;
        }
        if (word.starts_with(wordToMatch)) {
            return true;
        }
    }
    return false;
}

std::u16string PlGrammarSynthesizer::getPreposition(const std::u16string& preposition, std::u16string_view word)
{
    auto *foundPreposition = inflection::util::ArrayUtils::searchSorted<PREPOSITION_MAP>(preposition,
        [](const auto& item) { return item.preposition; });
    if (foundPreposition != nullptr) {
        std::u16string lowerCasedWord;
        inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, inflection::util::LocaleUtils::POLISH());
        const auto& matchingPrepositions = *foundPreposition;
        if (lowerCasedWord.length() > 1 && ((std::u16string_view(matchingPrepositions.prefixLetters).find(lowerCasedWord[0]) != std::string::npos
            && !inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(lowerCasedWord[1]))
            || matchesPrefix(lowerCasedWord, std::span(matchingPrepositions.prefixes))
            || matchesWord(lowerCasedWord, std::span(matchingPrepositions.words))))
        {
            return preposition + u"e";
        }
    }
    return preposition;
}

std::u16string PlGrammarSynthesizer::getPreposition(const inflection::tokenizer::Token* tok)
{
    std::u16string preposition(npc(tok)->getValue());
    auto significantToken = GrammarSynthesizerUtil::getFirstSignificantToken(npc(tok)->getNext());
    if (significantToken != nullptr) {
        preposition = PlGrammarSynthesizer::getPreposition(npc(tok)->getCleanValue(), npc(significantToken)->getCleanValue());
    }
    return preposition;
}

void PlGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::POLISH(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::POLISH(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::POLISH(), {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_VOCATIVE}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::ANIMACY, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::POLISH(), {GrammemeConstants::ANIMACY_ANIMATE, GrammemeConstants::ANIMACY_INANIMATE, GrammemeConstants::ANIMACY_HUMAN}));
    featureModel.setDefaultDisplayFunction(new PlGrammarSynthesizer_PlDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis
