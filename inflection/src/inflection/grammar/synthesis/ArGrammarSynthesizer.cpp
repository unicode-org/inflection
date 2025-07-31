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
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

void ArGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_WITH, new ArGrammarSynthesizer_WithPrepositionLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_TO, new ArGrammarSynthesizer_ToPrepositionLookupFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_AS, new ArGrammarSynthesizer_AsPrepositionLookupFunction(featureModel));
    ::std::map<int32_t, ::std::u16string_view> pronounPossessive;
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::masculine, Person::first).value, u"ي");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::feminine, Person::first).value, u"ي");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::masculine, Person::first).value, u"نا");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::feminine, Person::first).value, u"نا");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::masculine, Person::second).value, u"ك");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::feminine, Person::second).value, u"ك");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::dual, PronounGender::masculine, Person::second).value, u"كما");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::dual, PronounGender::feminine, Person::second).value, u"كما");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::masculine, Person::second).value, u"كم");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::feminine, Person::second).value, u"كن");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::masculine, Person::third).value, u"ه");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::singular, PronounGender::feminine, Person::third).value, u"ها");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::dual, PronounGender::masculine, Person::third).value, u"هما");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::dual, PronounGender::feminine, Person::third).value, u"هما");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::masculine, Person::third).value, u"هم");
    pronounPossessive.emplace(makeLookupKey(PronounNumber::plural, PronounGender::feminine, Person::third).value, u"هن");
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_POSSESSIVE, new ArGrammarSynthesizer_PronounLookupFunction(featureModel, pronounPossessive));
    featureModel.setDefaultDisplayFunction(new ArGrammarSynthesizer_ArDisplayFunction(featureModel, pronounPossessive));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ARABIC(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ARABIC(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL(), GrammemeConstants::NUMBER_DUAL()}, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ArGrammarSynthesizer_DefinitenessLookupFunction(pronounPossessive));
}

ArGrammarSynthesizer::PronounNumber ArGrammarSynthesizer::getPronounNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR()) {
            return PronounNumber::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_DUAL()) {
            return PronounNumber::dual;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL()) {
            return PronounNumber::plural;
        }
    }
    return PronounNumber::undefined;
}

ArGrammarSynthesizer::Person ArGrammarSynthesizer::getPerson(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::PERSON_FIRST()) {
            return Person::first;
        }
        if (valRef == GrammemeConstants::PERSON_SECOND()) {
            return Person::second;
        }
        if (valRef == GrammemeConstants::PERSON_THIRD()) {
            return Person::third;
        }
    }
    return Person::undefined;
}

ArGrammarSynthesizer::PronounGender ArGrammarSynthesizer::getPronounGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE()) {
            return PronounGender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE()) {
            return PronounGender::feminine;
        }
    }
    return PronounGender::undefined;
}

ArGrammarSynthesizer::LookupKey ArGrammarSynthesizer::makeLookupKey(PronounNumber field0, PronounGender field1, Person field2) {
    LookupKey retVal {.value = 0};
    retVal.fields.field0 = (uint8_t)field0;
    retVal.fields.field1 = (uint8_t)field1;
    retVal.fields.field2 = (uint8_t)field2;
    return retVal;
}

ArGrammarSynthesizer::LookupKey ArGrammarSynthesizer::updateLookupKeyAttribute(LookupKey lookupKey, const ::std::u16string& attribute, const ::std::u16string* value) {
    if (attribute == GrammemeConstants::NUMBER) {
        lookupKey.fields.field0 = (uint8_t)getPronounNumber(value);
    } else if (attribute == GrammemeConstants::GENDER) {
        lookupKey.fields.field1 = (uint8_t)getPronounGender(value);
    } else if (attribute == GrammemeConstants::PERSON) {
        lookupKey.fields.field2 = (uint8_t)getPerson(value);
    }

    return lookupKey;
}

} // namespace inflection::grammar::synthesis
