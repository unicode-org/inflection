/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */

#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_NumberLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_CaseLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>
#include <map>

namespace inflection::grammar::synthesis {

    static constexpr auto MOOD_SUBJUNCTIVE = u"subjunctive";

void MlGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER,
        new MlGrammarSynthesizer_NumberLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER,
        new MlGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE,
        new MlGrammarSynthesizer_CaseLookupFunction());

    featureModel.setDefaultDisplayFunction(new MlGrammarSynthesizer_MlDisplayFunction(featureModel));
}

MlGrammarSynthesizer::Number MlGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (!value) return Number::undefined;
    if (*value == GrammemeConstants::NUMBER_SINGULAR()) return Number::singular;
    if (*value == GrammemeConstants::NUMBER_PLURAL())   return Number::plural;
    return Number::undefined;
}

MlGrammarSynthesizer::Case MlGrammarSynthesizer::getCase(const ::std::u16string* value) {
    if (!value) return Case::undefined;
    if (*value == GrammemeConstants::CASE_NOMINATIVE())   return Case::nominative;
    if (*value == GrammemeConstants::CASE_ACCUSATIVE())   return Case::accusative;
    if (*value == GrammemeConstants::CASE_DATIVE())       return Case::dative;
    if (*value == GrammemeConstants::CASE_GENITIVE())     return Case::genitive;
    if (*value == GrammemeConstants::CASE_INSTRUMENTAL()) return Case::instrumental;
    if (*value == GrammemeConstants::CASE_LOCATIVE())     return Case::locative;
    return Case::undefined;
}

MlGrammarSynthesizer::Person MlGrammarSynthesizer::getPerson(const ::std::u16string* value) {
    if (!value) return Person::undefined;
    if (*value == GrammemeConstants::PERSON_FIRST())  return Person::first;
    if (*value == GrammemeConstants::PERSON_SECOND()) return Person::second;
    if (*value == GrammemeConstants::PERSON_THIRD())  return Person::third;
    return Person::undefined;
}

MlGrammarSynthesizer::Tense MlGrammarSynthesizer::getTense(const ::std::u16string* value) {
    if (!value) return Tense::undefined;
    if (*value == GrammemeConstants::TENSE_PAST())    return Tense::past;
    if (*value == GrammemeConstants::TENSE_PRESENT()) return Tense::present;
    if (*value == GrammemeConstants::TENSE_FUTURE())  return Tense::future;
    return Tense::undefined;
}

MlGrammarSynthesizer::Mood MlGrammarSynthesizer::getMood(const ::std::u16string* value) {
    if (!value) return Mood::undefined;
    if (*value == GrammemeConstants::MOOD_INDICATIVE())  return Mood::indicative;
    if (*value == GrammemeConstants::MOOD_IMPERATIVE())  return Mood::imperative;
    if (*value == MOOD_SUBJUNCTIVE) return Mood::subjunctive;
    return Mood::undefined;
}

MlGrammarSynthesizer::LookupKey MlGrammarSynthesizer::makeLookupKey(Number num, Case kase) {
    return (static_cast<LookupKey>(kase) & 0xFF)
         | ((static_cast<LookupKey>(num)  & 0xFF) << 8);
}

MlGrammarSynthesizer::LookupKey MlGrammarSynthesizer::makeVerbLookupKey(Person person, Number num, Tense tense, Mood mood) {
    return (static_cast<LookupKey>(person) & 0xFF)
         | ((static_cast<LookupKey>(num)    & 0xFF) << 8)
         | ((static_cast<LookupKey>(tense)  & 0x0F) << 24)
         | ((static_cast<LookupKey>(mood)   & 0x0F) << 28);
}

MlGrammarSynthesizer::Person MlGrammarSynthesizer::personFromConstraint(const ::std::u16string& val) {
    return getPerson(&val);
}

MlGrammarSynthesizer::Number MlGrammarSynthesizer::numberFromConstraint(const ::std::u16string& val) {
    return getNumber(&val);
}

MlGrammarSynthesizer::Case MlGrammarSynthesizer::caseFromConstraint(const ::std::u16string& val) {
    return getCase(&val);
}

MlGrammarSynthesizer::LookupKey MlGrammarSynthesizer::buildVerbSuffixKey(const std::vector<::std::u16string>& constraintValues) {
    Person person = Person::undefined;
    Number num    = Number::undefined;
    Tense tense   = Tense::undefined;
    Mood mood     = Mood::undefined;

    for (const auto& val : constraintValues) {
        if (person == Person::undefined) person = personFromConstraint(val);
        if (num    == Number::undefined) num    = numberFromConstraint(val);
        if (tense  == Tense::undefined)  tense  = getTense(&val);
        if (mood   == Mood::undefined)   mood   = getMood(&val);
    }

    return makeVerbLookupKey(person, num, tense, mood);
}

const std::map<MlGrammarSynthesizer::LookupKey, ::std::u16string> MlGrammarSynthesizer::malayalamSuffixMap = {
    {makeLookupKey(Number::singular, Case::nominative), u""},
    {makeLookupKey(Number::plural,   Case::nominative), u"കൾ"},
    {makeLookupKey(Number::singular, Case::genitive),   u"യുടെ"},
    {makeLookupKey(Number::plural,   Case::genitive),   u"കളുടെ"},
    {makeLookupKey(Number::singular, Case::dative),    u"ക്ക്"},
    {makeLookupKey(Number::plural,   Case::dative),    u"കൾക്ക്"},
};

const std::map<MlGrammarSynthesizer::LookupKey, ::std::u16string> MlGrammarSynthesizer::malayalamVerbSuffixMap = {
    {makeVerbLookupKey(Person::first,  Number::singular, Tense::past,    Mood::indicative), u"ച്ചു"},
    {makeVerbLookupKey(Person::first,  Number::plural,   Tense::past,    Mood::indicative), u"ഞ്ഞു"},
    {makeVerbLookupKey(Person::second, Number::singular, Tense::past,    Mood::indicative), u"ച്ചു"},
    {makeVerbLookupKey(Person::second, Number::plural,   Tense::past,    Mood::indicative), u"ന്നു"},
    {makeVerbLookupKey(Person::third,  Number::singular, Tense::past,    Mood::indicative), u"ച്ചു"},
    {makeVerbLookupKey(Person::third,  Number::plural,   Tense::past,    Mood::indicative), u"ന്നു"},

    {makeVerbLookupKey(Person::first,  Number::singular, Tense::present, Mood::indicative), u"ിക്കുന്നു"},
    {makeVerbLookupKey(Person::first,  Number::plural,   Tense::present, Mood::indicative), u"ിക്കുന്നു"},
    {makeVerbLookupKey(Person::second, Number::singular, Tense::present, Mood::indicative), u"ിക്കുന്നു"},
    {makeVerbLookupKey(Person::second, Number::plural,   Tense::present, Mood::indicative), u"ിക്കുന്നു"},
    {makeVerbLookupKey(Person::third,  Number::singular, Tense::present, Mood::indicative), u"ിക്കുന്നു"},
    {makeVerbLookupKey(Person::third,  Number::plural,   Tense::present, Mood::indicative), u"ിക്കുന്നു"},

    {makeVerbLookupKey(Person::first,  Number::singular, Tense::future,  Mood::indicative), u" ചെയ്യും"},
    {makeVerbLookupKey(Person::first,  Number::plural,   Tense::future,  Mood::indicative), u" ചെയ്യും"},
    {makeVerbLookupKey(Person::second, Number::singular, Tense::future,  Mood::indicative), u" ചെയ്യും"},
    {makeVerbLookupKey(Person::second, Number::plural,   Tense::future,  Mood::indicative), u" ചെയ്യും"},
    {makeVerbLookupKey(Person::third,  Number::singular, Tense::future,  Mood::indicative), u" ചെയ്യും"},
    {makeVerbLookupKey(Person::third,  Number::plural,   Tense::future,  Mood::indicative), u" ചെയ്യും"},
};

const std::u16string& MlGrammarSynthesizer::getSuffix(LookupKey key) {
    static const std::u16string empty = u"";
    auto it = malayalamSuffixMap.find(key);
    return it != malayalamSuffixMap.end() ? it->second : empty;
}

const std::u16string& MlGrammarSynthesizer::getVerbSuffix(LookupKey key) {
    static const std::u16string empty = u"";
    auto it = malayalamVerbSuffixMap.find(key);
    return it != malayalamVerbSuffixMap.end() ? it->second : empty;
}

} // namespace inflection::grammar::synthesis
