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
#include <inflection/util/ArrayUtils.hpp>

namespace inflection::grammar::synthesis {

void MlGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new MlGrammarSynthesizer_NumberLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new MlGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new MlGrammarSynthesizer_CaseLookupFunction());

    featureModel.setDefaultDisplayFunction(new MlGrammarSynthesizer_MlDisplayFunction(featureModel));
}

MlGrammarSynthesizer::Number MlGrammarSynthesizer::getNumber(const ::std::u16string& value) {
    if (value == GrammemeConstants::NUMBER_SINGULAR) {
        return Number::singular;
    }
    if (value == GrammemeConstants::NUMBER_PLURAL) {
        return Number::plural;
    }
    return Number::undefined;
}

MlGrammarSynthesizer::Case MlGrammarSynthesizer::getCase(const ::std::u16string& value) {
    if (value == GrammemeConstants::CASE_NOMINATIVE) {
        return Case::nominative;
    }
    if (value == GrammemeConstants::CASE_ACCUSATIVE) {
        return Case::accusative;
    }
    if (value == GrammemeConstants::CASE_DATIVE) {
        return Case::dative;
    }
    if (value == GrammemeConstants::CASE_GENITIVE) {
        return Case::genitive;
    }
    if (value == GrammemeConstants::CASE_INSTRUMENTAL) {
        return Case::instrumental;
    }
    if (value == GrammemeConstants::CASE_LOCATIVE) {
        return Case::locative;
    }
    return Case::undefined;
}

MlGrammarSynthesizer::Person MlGrammarSynthesizer::getPerson(const ::std::u16string& value) {
    if (value == GrammemeConstants::PERSON_FIRST) {
        return Person::first;
    }
    if (value == GrammemeConstants::PERSON_SECOND) {
        return Person::second;
    }
    if (value == GrammemeConstants::PERSON_THIRD) {
        return Person::third;
    }
    return Person::undefined;
}

MlGrammarSynthesizer::Tense MlGrammarSynthesizer::getTense(const ::std::u16string& value) {
    if (value == GrammemeConstants::TENSE_PAST) {
        return Tense::past;
    }
    if (value == GrammemeConstants::TENSE_PRESENT) {
        return Tense::present;
    }
    if (value == GrammemeConstants::TENSE_FUTURE) {
        return Tense::future;
    }
    return Tense::undefined;
}

MlGrammarSynthesizer::Mood MlGrammarSynthesizer::getMood(const ::std::u16string& value) {
    if (value == GrammemeConstants::MOOD_INDICATIVE) {
        return Mood::indicative;
    }
    if (value == GrammemeConstants::MOOD_IMPERATIVE) {
        return Mood::imperative;
    }
    if (value == GrammemeConstants::MOOD_SUBJUNCTIVE) {
        return Mood::subjunctive;
    }
    return Mood::undefined;
}

MlGrammarSynthesizer::LookupKey MlGrammarSynthesizer::buildVerbSuffixKey(const std::vector<::std::u16string>& constraintValues) {
    Person person = Person::undefined;
    Number num    = Number::undefined;
    Tense tense   = Tense::undefined;
    Mood mood     = Mood::undefined;

    for (const auto& val : constraintValues) {
        if (person == Person::undefined) {
            person = getPerson(val);
        }
        if (num == Number::undefined) {
            num = getNumber(val);
        }
        if (tense == Tense::undefined) {
            tense = getTense(val);
        }
        if (mood == Mood::undefined) {
            mood = getMood(val);
        }
    }

    return makeVerbLookupKey(person, num, tense, mood);
}

typedef struct {
    MlGrammarSynthesizer::LookupKey key;
    const char16_t* suffix;
} KeyToSuffix;

static constexpr KeyToSuffix MALAYALAM_SUFFIX_MAP[] = {
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Case::nominative), u""},
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Case::dative),     u"ക്ക്"},
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Case::genitive),   u"യുടെ"},
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Case::nominative), u"കൾ"},
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Case::dative),     u"കൾക്ക്"},
    {MlGrammarSynthesizer::makeLookupKey(MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Case::genitive),   u"കളുടെ"},
};

static constexpr KeyToSuffix MALAYALAM_VERB_SUFFIX_MAP[] = {
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ച്ചു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ച്ചു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ച്ചു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ഞ്ഞു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::past,    MlGrammarSynthesizer::Mood::indicative), u"ന്നു"},

    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::present, MlGrammarSynthesizer::Mood::indicative), u"ിക്കുന്നു"},

    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::singular, MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::first,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::second, MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
    {MlGrammarSynthesizer::makeVerbLookupKey(MlGrammarSynthesizer::Person::third,  MlGrammarSynthesizer::Number::plural,   MlGrammarSynthesizer::Tense::future,  MlGrammarSynthesizer::Mood::indicative), u" ചെയ്യും"},
};

std::u16string_view MlGrammarSynthesizer::getSuffix(LookupKey key) {
    auto entry = inflection::util::ArrayUtils::searchSorted<MALAYALAM_SUFFIX_MAP>(key, [](const auto& item) { return item.key; });
    return entry != nullptr ? entry->suffix : std::u16string_view();
}

std::u16string_view MlGrammarSynthesizer::getVerbSuffix(LookupKey key) {
    auto entry = inflection::util::ArrayUtils::searchSorted<MALAYALAM_VERB_SUFFIX_MAP>(key, [](const auto& item) { return item.key; });
    return entry != nullptr ? entry->suffix : std::u16string_view();
}

} // namespace inflection::grammar::synthesis
