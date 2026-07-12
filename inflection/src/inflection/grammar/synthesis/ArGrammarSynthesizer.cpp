/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArDisplayFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_DefinitenessLookupFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_AsPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_PronounLookupFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ToPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_WithPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void ArGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_WITH, new ArGrammarSynthesizer_WithPrepositionLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_TO, new ArGrammarSynthesizer_ToPrepositionLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_AS, new ArGrammarSynthesizer_AsPrepositionLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_POSSESSIVE, new ArGrammarSynthesizer_PronounLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ARABIC(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE}, {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ARABIC(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL, GrammemeConstants::NUMBER_DUAL}, {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ArGrammarSynthesizer_DefinitenessLookupFunction());

    featureModel.setDefaultDisplayFunction(new ArGrammarSynthesizer_ArDisplayFunction(featureModel));
}

ArGrammarSynthesizer::Number ArGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_DUAL) {
            return Number::dual;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

ArGrammarSynthesizer::Gender ArGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE) {
            return Gender::feminine;
        }
    }
    return Gender::undefined;
}

ArGrammarSynthesizer::Person ArGrammarSynthesizer::getPerson(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::PERSON_FIRST) {
            return Person::first;
        }
        if (valRef == GrammemeConstants::PERSON_SECOND) {
            return Person::second;
        }
        if (valRef == GrammemeConstants::PERSON_THIRD) {
            return Person::third;
        }
    }
    return Person::undefined;
}

static constexpr struct {
    ArGrammarSynthesizer::LookupKey key;
    const char16_t* pronoun;
} POSSESSIVE_PRONOUNS[] = {
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::dual, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::second), u"كما"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::dual, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::third), u"هما"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::dual, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::second), u"كما"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::dual, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::third), u"هما"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::first), u"نا"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::second), u"كن"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::third), u"هن"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::first), u"نا"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::second), u"كم"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::plural, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::third), u"هم"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::first), u"ي"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::second), u"ك"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::feminine, ArGrammarSynthesizer::Person::third), u"ها"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::first), u"ي"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::second), u"ك"},
    {ArGrammarSynthesizer::makeLookupKey(ArGrammarSynthesizer::Number::singular, ArGrammarSynthesizer::Gender::masculine, ArGrammarSynthesizer::Person::third), u"ه"},
};

const char16_t* ArGrammarSynthesizer::getPossessivePronoun(LookupKey key) {
    auto entry = inflection::util::ArrayUtils::searchSorted<POSSESSIVE_PRONOUNS>(key,
            [](const auto& item) { return item.key; });
    if (entry != nullptr) {
        return entry->pronoun;
    }
    return nullptr;
}

std::set<std::u16string_view> ArGrammarSynthesizer::getPossessivePronouns() {
    std::set<std::u16string_view> pronouns;
    for (const auto& [key, pronoun] : POSSESSIVE_PRONOUNS) {
        pronouns.insert(pronoun);
    }
    return pronouns;
}

} // namespace inflection::grammar::synthesis
