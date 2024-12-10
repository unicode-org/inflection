/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_ArDisplayFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/npc.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace morphuntion::grammar::synthesis {

ArGrammarSynthesizer_ArDisplayFunction::ArGrammarSynthesizer_ArDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& personalPronounsMap)
    : super()
    , dictionaryInflector(::morphuntion::util::LocaleUtils::ARABIC(), {
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_SECOND(), GrammemeConstants::PERSON_FIRST()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_DUAL(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::DEFINITENESS_INDEFINITE(), GrammemeConstants::DEFINITENESS_CONSTRUCT(), GrammemeConstants::DEFINITENESS_DEFINITE()}
    }, {}, false)
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , pronounCountFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_NUMBER)))
    , pronounGenderFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_GENDER)))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::ARABIC()))
    , personalPronounsMap(personalPronounsMap)
{
    ::morphuntion::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&posMask, {u"noun", u"adjective", u"verb", u"determiner"}));
    ::morphuntion::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&verbMask, {u"verb"}));
    ::morphuntion::util::Validate::notNull(dictionaryInflector.getDictionary().getBinaryProperties(&determinerMask, {u"determiner"}));
    for (auto const& x : personalPronounsMap) {
        personalPronounsReverseMap[x.second] = x.first;
    }
}

static const char16_t* const IRREGULAR_NOUNS[] = {
    u"أخ",
    u"أب"
};

static constexpr char16_t AL[] = u"ال";
static constexpr char16_t AL_WITH_KASHEDA[] = u"الـ\u00A0";

namespace {
    std::map<std::u16string_view, std::u16string_view>* generateCaseConstraintFallbackMap() {
        auto* result = new std::map<std::u16string_view, std::u16string_view>();
        result->insert({GrammemeConstants::CASE_GENITIVE(), GrammemeConstants::CASE_ACCUSATIVE()});
        result->insert({GrammemeConstants::CASE_ACCUSATIVE(), GrammemeConstants::CASE_NOMINATIVE()});
        return result;
    }

    const std::map<std::u16string_view, std::u16string_view>& CASE_CONSTRAINT_FALLBACK() {
        static auto* result = generateCaseConstraintFallbackMap();
        return *result;
    }

    void getFeatureValue(::std::u16string* returnValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::dialog::SemanticFeature& semanticFeature){
        if (const auto result = constraints.find(semanticFeature); result != constraints.end()) {
            *npc(returnValue) = result->second;
        }
    }

    bool hasFeature(const ::std::u16string& value, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::dialog::SemanticFeature& semanticFeature){
        auto result = constraints.find(semanticFeature);
        return result != constraints.end() && result->second == value;
    }
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::performPronounInflection(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const
{
    auto possessiveException = personalPronounsReverseMap.find(word);
    ArGrammarSynthesizer::LookupKey pronounLookupKey {.value = (uint32_t) possessiveException->second};

    if (!inflectionContraints.countString.empty()) {
        pronounLookupKey = ArGrammarSynthesizer::updateLookupKeyAttribute(pronounLookupKey, GrammemeConstants::NUMBER, &inflectionContraints.countString);
    }
    if (!inflectionContraints.genderString.empty()) {
        pronounLookupKey = ArGrammarSynthesizer::updateLookupKeyAttribute(pronounLookupKey, GrammemeConstants::GENDER, &inflectionContraints.genderString);
    }

    auto modified_pronoun = personalPronounsMap.find(pronounLookupKey.value);

    if (modified_pronoun != personalPronounsMap.end()) {
        return modified_pronoun->second.data();
    }

    return word;
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::performInflection(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const
{
    const auto &dictionary = dictionaryInflector.getDictionary();
    int64_t wordProperties;
    dictionary.getCombinedBinaryType(&wordProperties, word);
    bool isVerb = (wordProperties & posMask) == verbMask;
    bool isDeterminer = (wordProperties & posMask) == determinerMask;

    ::std::vector<::std::u16string> constraints;
    if (!inflectionContraints.countString.empty()) {
        constraints.emplace_back(inflectionContraints.countString);
    } else {
        constraints.emplace_back(GrammemeConstants::NUMBER_SINGULAR());
    }
    if (!inflectionContraints.genderString.empty()) {
        constraints.emplace_back(inflectionContraints.genderString);
    }
    if (!inflectionContraints.definitenessString.empty() && GrammemeConstants::DEFINITENESS_CONSTRUCT() == inflectionContraints.definitenessString) {
        constraints.emplace_back(inflectionContraints.definitenessString);
    }
    if (!inflectionContraints.caseString.empty()) {
        constraints.emplace_back(inflectionContraints.caseString);
    } else if(!isVerb && !isDeterminer) {
        // We don't add case constraint for verbs by default
        constraints.emplace_back(GrammemeConstants::CASE_NOMINATIVE());
    }

    const auto &caseConstraintFallback = CASE_CONSTRAINT_FALLBACK();
    while (true) {
        const auto inflectedWord = dictionaryInflector.inflect(word, constraints, inflectionContraints.disambiguationGrammemeValues);
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

    if (!dictionary.isKnownWord(word)) {
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
        ::std::u16string inflectedChain;

        for (const auto& token : *tokenChain) {
            if (token.isSignificant()) {
                auto possessiveException = personalPronounsReverseMap.find(token.getValue());

                if (possessiveException != personalPronounsReverseMap.end()) {
                    inflectedChain += token.getValue();
                } else {
                    inflectedChain += performInflection(token.getValue(), inflectionContraints);
                }
            } else {
                inflectedChain += token.getValue();
            }
        }
        return inflectedChain;
    }

    return performInflection(word, inflectionContraints);
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * ArGrammarSynthesizer_ArDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    if (displayData.getValues().empty()) {
        return nullptr;
    }
    auto inflectionContraints = InflectionConstraints();

    getFeatureValue(&inflectionContraints.caseString, constraints, caseFeature);
    getFeatureValue(&inflectionContraints.countString, constraints, countFeature);
    getFeatureValue(&inflectionContraints.genderString, constraints, genderFeature);
    getFeatureValue(&inflectionContraints.definitenessString, constraints, definitenessFeature);
    getFeatureValue(&inflectionContraints.pronounCountString, constraints, pronounCountFeature);
    getFeatureValue(&inflectionContraints.pronounGenderString, constraints, pronounGenderFeature);
    getFeatureValue(&inflectionContraints.partOfSpeechString, constraints, partOfSpeechFeature);
    inflectionContraints.disambiguationGrammemeValues = GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature});

    auto dialogWord = displayData.getValues()[0].getDisplayString();
    for (const auto& value : displayData.getValues()) {
        auto valueConstraintMap = value.getConstraintMap();
        if ((inflectionContraints.caseString.empty() || hasFeature(inflectionContraints.caseString, valueConstraintMap, caseFeature) || valueConstraintMap.find(caseFeature) == valueConstraintMap.end())
            && (inflectionContraints.countString.empty() || hasFeature(inflectionContraints.countString, valueConstraintMap, countFeature))
            && (inflectionContraints.genderString.empty() || hasFeature(inflectionContraints.genderString, valueConstraintMap, genderFeature))
            && (inflectionContraints.definitenessString.empty() || hasFeature(inflectionContraints.definitenessString, valueConstraintMap, definitenessFeature) || (GrammemeConstants::DEFINITENESS_CONSTRUCT() == inflectionContraints.definitenessString && valueConstraintMap.find(definitenessFeature) == valueConstraintMap.end()) || (GrammemeConstants::DEFINITENESS_DEFINITE() == inflectionContraints.definitenessString && valueConstraintMap.find(definitenessFeature) == valueConstraintMap.end())))
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
        if (::morphuntion::util::StringViewUtils::trim(dialogWord).find(u' ') == ::std::u16string::npos
            && GrammemeConstants::DEFINITENESS_DEFINITE() == inflectionContraints.definitenessString)
        {
            displayString = lookupDefiniteArticle(dialogWord);
        }
        if (!inflectionContraints.caseString.empty() && (IRREGULAR_NOUNS[0] == displayString || IRREGULAR_NOUNS[1] == displayString)) {
            if (GrammemeConstants::CASE_NOMINATIVE() == inflectionContraints.caseString) {
                displayString += u"و";
            } else if (GrammemeConstants::CASE_GENITIVE() == inflectionContraints.caseString) {
                displayString += u"ي";
            } else if (GrammemeConstants::CASE_ACCUSATIVE() == inflectionContraints.caseString) {
                displayString += u"ا";
            }
        }
        return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString);
    }
    if (GrammemeConstants::DEFINITENESS_DEFINITE() == inflectionContraints.definitenessString && ::morphuntion::util::StringViewUtils::trim(dialogWord).find(u' ') == ::std::u16string::npos) {
        inflection = lookupDefiniteArticle(inflection);
    }
    return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(inflection, constraints);
}

::std::u16string ArGrammarSynthesizer_ArDisplayFunction::lookupDefiniteArticle(const ::std::u16string& displayString)
{
    std::u16string trimmedDisplayString(::morphuntion::util::StringViewUtils::trim(displayString));
    if (trimmedDisplayString.empty()) {
        return displayString;
    }
    if (morphuntion::util::StringViewUtils::startsWith(trimmedDisplayString, AL)) {
        return trimmedDisplayString;
    }
    ::std::u16string article;
    if (::morphuntion::lang::StringFilterUtil::ARABIC_SCRIPT().contains(trimmedDisplayString[0])) {
        article = AL;
    }
    else {
        article = AL_WITH_KASHEDA;
    }
    return article + trimmedDisplayString;
}

} // namespace morphuntion::grammar::synthesis
