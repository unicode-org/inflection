/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/Validate.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlNounInflectionPattern::NlGrammarSynthesizer_NlNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::DUTCH()))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , declensionFeature(*npc(model.getFeature(NlGrammarSynthesizer::DECLENSION)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DUTCH())))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {GrammemeConstants::POS_NOUN}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL}));
}

NlGrammarSynthesizer_NlNounInflectionPattern::~NlGrammarSynthesizer_NlNounInflectionPattern() = default;

static bool isCountConditionalPronoun(std::u16string_view word) {
    static const char16_t* countConditionalPronouns[] = {
        u"ze",
        u"zij",
    };
    for (auto pronoun : countConditionalPronouns) {
        if (word == pronoun) {
            return true;
        }
    }

    return false;
}

static bool isDeclensionConditional(std::u16string_view word) {
    static const char16_t* declensionConditional[] = {
        u"ons",
        u"we",
        u"wij",
    };
    for (auto pronoun : declensionConditional) {
        if (word == pronoun) {
            return true;
        }
    }

    return false;
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::isGenitiveRequested(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    // Only support nominative --> genitive
    return (NlGrammarSynthesizer::getCase(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature)) == NlGrammarSynthesizer::Case::genitive)
        && (!isCountConditionalPronoun(displayString) || getGrammaticalNumber(constraints) != NlGrammarSynthesizer::Number::undefined)
        && (!isDeclensionConditional(displayString) || getDeclension(constraints) != NlGrammarSynthesizer::Declension::undefined);
}

::std::u16string NlGrammarSynthesizer_NlNounInflectionPattern::inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (isGenitiveRequested(displayString, constraints)) {
        return inflectGenitive(displayString, constraints);
    }

    auto requestedPOS(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));
    auto targetCount = getGrammaticalNumber(constraints);
    if (targetCount == NlGrammarSynthesizer::Number::undefined
        || (wordGrammemes & dictionaryNoun) != dictionaryNoun
        || (!requestedPOS.empty() && requestedPOS != GrammemeConstants::POS_NOUN))
    {
        return {};
    }
    if (constraints.size() == 1
        && ((NlGrammarSynthesizer::Number::singular == targetCount && (wordGrammemes & dictionarySingular) == dictionarySingular)
        || (NlGrammarSynthesizer::Number::plural == targetCount && (wordGrammemes & dictionaryPlural) == dictionaryPlural)))
    {
        // The requested inflection is already the provided form.
        return displayString;
    }
    std::vector<dictionary::Inflector_InflectionPattern> patterns;
    inflector.getInflectionPatternsForWord(displayString, patterns);
    ::std::u16string inflection;
    if (!patterns.empty()) {
        int64_t toConstraintGrammemes;
        if (targetCount == NlGrammarSynthesizer::Number::singular) {
            toConstraintGrammemes = dictionarySingular;
        }
        else {
            toConstraintGrammemes = dictionaryPlural;
        }
        for (const auto& pattern : patterns) {
            if ((pattern.getPartsOfSpeech() & dictionaryNoun) == 0) {
                continue;
            }
            inflection = pattern.reinflect(wordGrammemes, toConstraintGrammemes, displayString);
            if (!inflection.empty()) {
                return inflection;
            }
            // Failed to inflect.
        }
    }
    return inflection;
}

static constexpr struct {
    const char16_t* pronoun;
    const char16_t* possessivePronoun;
} possessivePronouns[] = {
    // This must be in sorted order.
    {u"haar", u"haar"},
    {u"hem", u"zijn"},
    {u"hen", u"hun"},
    {u"hij", u"zijn"},
    {u"hun", u"hun"},
    {u"ik", u"mijn"},
    {u"je", u"jouw"},
    {u"jij", u"jouw"},
    {u"jou", u"jouw"},
    {u"me", u"mijn"},
    {u"mij", u"mijn"},
    // Too little information for the following.
    // {u"zij", u"hun"},   // need count
    // {u"zij", u"haar"},  // need count
    // {u"ze", u"hun"},    // need count
    // {u"ze", u"haar"},   // need count
    // {u"we", u"ons"},    // need declension
    // {u"wij", u"ons"},   // need declension
    // {u"ons", u"ons"},   // need declension
};

::std::u16string NlGrammarSynthesizer_NlNounInflectionPattern::inflectGenitive(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto possessiveException = inflection::util::ArrayUtils::searchSorted<possessivePronouns>(displayString,
        [](const auto& item) { return item.pronoun; });
    if (possessiveException != nullptr) {
        return possessiveException->possessivePronoun;
    }

    if (isCountConditionalPronoun(displayString)) {
        switch (getGrammaticalNumber(constraints)) {
            case NlGrammarSynthesizer::Number::singular: return u"haar";
            case NlGrammarSynthesizer::Number::plural: return u"hun";
            default: return {};
        }
    }

    if (isDeclensionConditional(displayString)) {
        switch (getDeclension(constraints)) {
            case NlGrammarSynthesizer::Declension::undeclined: return u"ons";
            case NlGrammarSynthesizer::Declension::declined: return u"onze";
            default: return {};
        }
    }

    // If not a pronoun, assume it's a name
    if (displayString.length() > 2) {
        if (isPossessive(displayString)) {
            return displayString;
        }
        auto last = displayString[displayString.length()-1];
        if (isApostrophe(last)) {
            return displayString + u"s";
        } else if (endsWithDutchSSound(displayString)) {
            return displayString + u"’";
        } else {
            return displayString + u"’s";
        }
    }
    return {};
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::isApostrophe(char16_t c) const
{
    return apostrophes.find(c) != std::u16string_view::npos;
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::isPossessive(const ::std::u16string& text) const
{
    if (text.length() > 2) {
        ::std::u16string lowerCaseText;
        ::inflection::util::StringViewUtils::lowercase(&lowerCaseText, text, ::inflection::util::LocaleUtils::DUTCH());
        auto secondToLast = lowerCaseText[lowerCaseText.length()-2];
        auto last = lowerCaseText[lowerCaseText.length()-1];

        // 's
        if (isApostrophe(secondToLast) && last == u's') {
            return true;
        }
        if (isApostrophe(last)) {
            return endsWithDutchSSound(lowerCaseText.substr(0, lowerCaseText.length()-1));
        }
    }
    return false;
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::endsWithDutchSSound(const ::std::u16string& text)
{
    ::std::u16string lowerCaseText;
    ::inflection::util::StringViewUtils::lowercase(&lowerCaseText, text, ::inflection::util::LocaleUtils::DUTCH());

    return lowerCaseText.ends_with(u"s")
        || lowerCaseText.ends_with(u"z")
        || lowerCaseText.ends_with(u"sch")
        || lowerCaseText.ends_with(u"x")
        || lowerCaseText.ends_with(u"sh");
}

NlGrammarSynthesizer::Number
NlGrammarSynthesizer_NlNounInflectionPattern::getGrammaticalNumber(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    return NlGrammarSynthesizer::getNumber(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
}

NlGrammarSynthesizer::Declension
NlGrammarSynthesizer_NlNounInflectionPattern::getDeclension(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    return NlGrammarSynthesizer::getDeclension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
}

} // namespace inflection::grammar::synthesis
