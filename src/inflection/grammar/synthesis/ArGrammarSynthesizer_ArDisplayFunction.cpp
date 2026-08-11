/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArDisplayFunction.hpp>

#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/npc.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_ArDisplayFunction::ArGrammarSynthesizer_ArDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionaryInflector(::inflection::util::LocaleUtils::ARABIC(), {
            {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB},
            {GrammemeConstants::PERSON_THIRD, GrammemeConstants::PERSON_SECOND, GrammemeConstants::PERSON_FIRST},
            {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE},
            {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_DUAL, GrammemeConstants::NUMBER_PLURAL},
            {GrammemeConstants::DEFINITENESS_INDEFINITE, GrammemeConstants::DEFINITENESS_CONSTRUCT, GrammemeConstants::DEFINITENESS_DEFINITE}
    }, {}, false)
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , moodFeature(*npc(model.getFeature(GrammemeConstants::MOOD)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , tenseFeature(*npc(model.getFeature(GrammemeConstants::TENSE)))
    , personFeature(*npc(model.getFeature(GrammemeConstants::PERSON)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , animacyFeature(*npc(model.getFeature(GrammemeConstants::ANIMACY)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , pronounCountFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_NUMBER)))
    , pronounGenderFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_GENDER)))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ARABIC()))
    , personalPronouns(ArGrammarSynthesizer::getPossessivePronouns())
    , caseConstraintFallback({
        {GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_ACCUSATIVE},
        {GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_NOMINATIVE}
    })
{
    ::inflection::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&posMask, {u"noun", u"adjective", u"verb", u"determiner"}));
    ::inflection::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&verbMask, {u"verb"}));
    ::inflection::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&determinerMask, {u"determiner"}));
}

static const char16_t* const IRREGULAR_NOUNS[] = {
    u"أخ",
    u"أب"
};

static constexpr char16_t AL[] = u"ال";
static constexpr char16_t AL_WITH_KASHEDA[] = u"الـ ";

namespace {
    void getFeatureValue(::std::u16string* returnValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature& semanticFeature){
        if (const auto result = constraints.find(semanticFeature); result != constraints.end()) {
            *npc(returnValue) = result->second;
        }
    }

    bool hasFeature(const ::std::u16string& value, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature& semanticFeature){
        auto result = constraints.find(semanticFeature);
        return result != constraints.end() && result->second == value;
    }
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::performInflection(const ::std::u16string &word, int64_t wordGrammemes, const InflectionConstraints &inflectionContraints) const
{
    bool isVerb = (wordGrammemes & posMask) == verbMask;
    bool isDeterminer = (wordGrammemes & posMask) == determinerMask;

    ::std::vector<::std::u16string> constraints;
    if (!inflectionContraints.countString.empty()) {
        constraints.emplace_back(inflectionContraints.countString);
    } else {
        constraints.emplace_back(GrammemeConstants::NUMBER_SINGULAR);
    }
    if (!inflectionContraints.genderString.empty()) {
        constraints.emplace_back(inflectionContraints.genderString);
    }
    if (!inflectionContraints.animacyString.empty()) {
        constraints.emplace_back(inflectionContraints.animacyString);
    }
    if (!inflectionContraints.definitenessString.empty() && inflectionContraints.definitenessString == GrammemeConstants::DEFINITENESS_CONSTRUCT) {
        constraints.emplace_back(inflectionContraints.definitenessString);
    }
    if (!inflectionContraints.caseString.empty()) {
        constraints.emplace_back(inflectionContraints.caseString);
    } else if(!isVerb && !isDeterminer && inflectionContraints.partOfSpeechString != u"verb") {
        // We don't add case constraint for verbs by default
        constraints.emplace_back(GrammemeConstants::CASE_NOMINATIVE);
    }
    if (!inflectionContraints.personString.empty()) {
        constraints.emplace_back(inflectionContraints.personString);
    }
    while (true) {
        const auto inflectedWord = dictionaryInflector.inflect(word, wordGrammemes, constraints, inflectionContraints.disambiguationGrammemeValues);
        if (inflectedWord.has_value()) {
            return *inflectedWord;
        }
        if (isVerb) {
            // No case fallback happens for verbs
            return word;
        }
        const auto fallbackConstraintItem = caseConstraintFallback.find(constraints.back());
        if (fallbackConstraintItem == caseConstraintFallback.end()) {
            break;
        }
        constraints.back() = fallbackConstraintItem->second;
    }
    return word;
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::inflectString(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const
{
    const auto &dictionary = dictionaryInflector.getDictionary();
    int64_t wordGrammemes = 0;

    if (dictionary.getCombinedBinaryType(&wordGrammemes, word) != nullptr) {
        return performInflection(word, wordGrammemes, inflectionContraints);
    }

    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
    ::std::u16string inflectedChain;

    for (const auto& token : *tokenChain) {
        const auto& tokenVal = token.getValue();
        if (token.isSignificant() && !personalPronouns.contains(tokenVal)) {
            dictionary.getCombinedBinaryType(&wordGrammemes, tokenVal);
            inflectedChain += performInflection(tokenVal, wordGrammemes, inflectionContraints);
        } else {
            inflectedChain += tokenVal;
        }
    }
    return inflectedChain;
}

::inflection::dialog::DisplayValue * ArGrammarSynthesizer_ArDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    if (displayData.getValues().empty()) {
        return nullptr;
    }
    InflectionConstraints inflectionContraints;

    getFeatureValue(&inflectionContraints.caseString, constraints, caseFeature);
    getFeatureValue(&inflectionContraints.moodString, constraints, moodFeature);
    getFeatureValue(&inflectionContraints.countString, constraints, countFeature);
    getFeatureValue(&inflectionContraints.tenseString, constraints, tenseFeature);
    getFeatureValue(&inflectionContraints.personString, constraints, personFeature);
    getFeatureValue(&inflectionContraints.genderString, constraints, genderFeature);
    getFeatureValue(&inflectionContraints.animacyString, constraints, animacyFeature);
    getFeatureValue(&inflectionContraints.definitenessString, constraints, definitenessFeature);
    getFeatureValue(&inflectionContraints.pronounCountString, constraints, pronounCountFeature);
    getFeatureValue(&inflectionContraints.pronounGenderString, constraints, pronounGenderFeature);
    getFeatureValue(&inflectionContraints.partOfSpeechString, constraints, partOfSpeechFeature);
    inflectionContraints.disambiguationGrammemeValues = GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature, &moodFeature, &personFeature, &tenseFeature});

    auto dialogWord = displayData.getValues()[0].getDisplayString();
    for (const auto& value : displayData.getValues()) {
        const auto& valueConstraintMap = value.getConstraintMap();
        if ((inflectionContraints.caseString.empty() || hasFeature(inflectionContraints.caseString, valueConstraintMap, caseFeature) || !valueConstraintMap.contains(caseFeature))
            && (inflectionContraints.countString.empty() || hasFeature(inflectionContraints.countString, valueConstraintMap, countFeature))
            && (inflectionContraints.genderString.empty() || hasFeature(inflectionContraints.genderString, valueConstraintMap, genderFeature))
            && (inflectionContraints.definitenessString.empty() || hasFeature(inflectionContraints.definitenessString, valueConstraintMap, definitenessFeature) || (inflectionContraints.definitenessString == GrammemeConstants::DEFINITENESS_CONSTRUCT && !valueConstraintMap.contains(definitenessFeature)) || (inflectionContraints.definitenessString == GrammemeConstants::DEFINITENESS_DEFINITE && !valueConstraintMap.contains(definitenessFeature))))
        {
            dialogWord = value.getDisplayString();
        }
    }
    ::std::u16string inflection;
    if (inflectionContraints.pronounCountString.empty() && inflectionContraints.pronounGenderString.empty()) {
        inflection = inflectString(dialogWord, inflectionContraints);
    }
    if (inflection.empty()) {
        ::std::u16string displayString;
        displayString = dialogWord;
        if (::inflection::util::StringViewUtils::trim(dialogWord).find(u' ') == ::std::u16string::npos
            && inflectionContraints.definitenessString == GrammemeConstants::DEFINITENESS_DEFINITE)
        {
            displayString = lookupDefiniteArticle(dialogWord);
        }
        if (!inflectionContraints.caseString.empty() && (IRREGULAR_NOUNS[0] == displayString || IRREGULAR_NOUNS[1] == displayString)) {
            if (inflectionContraints.caseString == GrammemeConstants::CASE_NOMINATIVE) {
                displayString += u"و";
            } else if (inflectionContraints.caseString == GrammemeConstants::CASE_GENITIVE) {
                displayString += u"ي";
            } else if (inflectionContraints.caseString == GrammemeConstants::CASE_ACCUSATIVE) {
                displayString += u"ا";
            }
        }
        return new ::inflection::dialog::DisplayValue(displayString);
    }
    if (inflectionContraints.definitenessString == GrammemeConstants::DEFINITENESS_DEFINITE && ::inflection::util::StringViewUtils::trim(dialogWord).find(u' ') == ::std::u16string::npos) {
        inflection = lookupDefiniteArticle(inflection);
    }
    return new ::inflection::dialog::DisplayValue(inflection, constraints);
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::lookupDefiniteArticle(const ::std::u16string& displayString)
{
    std::u16string trimmedDisplayString(::inflection::util::StringViewUtils::trim(displayString));
    if (trimmedDisplayString.empty()) {
        return displayString;
    }
    if (trimmedDisplayString.starts_with(AL)) {
        return trimmedDisplayString;
    }
    ::std::u16string article;
    if (uscript_hasScript(trimmedDisplayString[0], USCRIPT_ARABIC)) {
        article = AL;
    }
    else {
        article = AL_WITH_KASHEDA;
    }
    return article + trimmedDisplayString;
}

} // namespace inflection::grammar::synthesis
