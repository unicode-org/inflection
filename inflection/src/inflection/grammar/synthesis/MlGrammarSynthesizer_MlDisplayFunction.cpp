/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/uchar.h>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

// Helper function to get string feature by name from the feature map
static std::u16string getStrFeature(
    const std::u16string& name,
    const std::map<dialog::SemanticFeature, std::u16string>& features) 
{
    for (const auto& [key, value] : features) {
        if (key.getName() == name) {
            return value;
        }
    }
    return u"";
}

// Changed parameter type from FeatureSet to map<SemanticFeature,u16string>
std::u16string fallbackMalayalamPronoun(
    const std::map<dialog::SemanticFeature, std::u16string>& features) 
{
    auto person = getStrFeature(u"person", features);
    auto number = getStrFeature(u"number", features);
    auto case_ = getStrFeature(u"case", features);
    auto gender = getStrFeature(u"gender", features);
    auto clusivity = getStrFeature(u"clusivity", features);
    auto formality = getStrFeature(u"formality", features);

    if (person.empty() || number.empty() || case_.empty()) {
        return u"";
    }
    if (person == u"first" && number == u"singular" && case_ == u"dative") {
        return u"എനിക്ക്";
    }
    return u"";
}

using dialog::SemanticFeature;
using dialog::SemanticFeatureModel_DisplayData;
using dialog::DisplayValue;

static constexpr auto CASE_NOMINATIVE = u"nominative";
static constexpr auto CASE_ACCUSATIVE = u"accusative";
static constexpr auto CASE_DATIVE = u"dative";
static constexpr auto CASE_GENITIVE = u"genitive";
static constexpr auto CASE_INSTRUMENTAL = u"instrumental";
static constexpr auto CASE_LOCATIVE = u"locative";
static constexpr auto CASE_SOCIATIVE = u"sociative";

static constexpr auto NUMBER_SINGULAR = u"singular";
static constexpr auto NUMBER_PLURAL = u"plural";

static constexpr auto GENDER_MASCULINE = u"masculine";
static constexpr auto GENDER_FEMININE = u"feminine";
static constexpr auto GENDER_NEUTER = u"neuter";

static constexpr auto FORMALITY_FORMAL = u"formal";
static constexpr auto FORMALITY_INFORMAL = u"informal";

static constexpr auto CLUSIVITY_INCLUSIVE = u"inclusive";
static constexpr auto CLUSIVITY_EXCLUSIVE = u"exclusive";

static constexpr auto PERSON_FIRST = u"first";
static constexpr auto PERSON_SECOND = u"second";
static constexpr auto PERSON_THIRD = u"third";

static constexpr auto TENSE_PAST = u"past";
static constexpr auto TENSE_PRESENT = u"present";
static constexpr auto TENSE_FUTURE = u"future";

static constexpr auto MOOD_INDICATIVE = u"indicative";
static constexpr auto MOOD_IMPERATIVE = u"imperative";
static constexpr auto MOOD_SUBJUNCTIVE = u"subjunctive";

MlGrammarSynthesizer_MlDisplayFunction::MlGrammarSynthesizer_MlDisplayFunction(
    const ::inflection::dialog::SemanticFeatureModel& model)
    : caseFeature(*npc(model.getFeature(GrammemeConstants::CASE))),
      numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER))),
      genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER))),
      posFeature(*npc(model.getFeature(GrammemeConstants::POS))),
      formalityFeature(*npc(model.getFeature(u"formality"))),
      clusivityFeature(*npc(model.getFeature(u"clusivity"))),
      personFeature(*npc(model.getFeature(GrammemeConstants::PERSON))),
      tenseFeature(*npc(model.getFeature(u"tense"))),
      moodFeature(*npc(model.getFeature(u"mood"))),
      pronounTypeFeature(*npc(model.getFeature(u"pronounType"))),
      determinationFeature(*npc(model.getFeature(u"determination"))),
      dictionaryInflector(
          util::LocaleUtils::MALAYALAM(),
          {
              {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB()},
              {CASE_NOMINATIVE, CASE_ACCUSATIVE, CASE_DATIVE, CASE_GENITIVE,
               CASE_LOCATIVE, CASE_INSTRUMENTAL, CASE_SOCIATIVE},
              {NUMBER_SINGULAR, NUMBER_PLURAL},
              {GENDER_MASCULINE, GENDER_FEMININE, GENDER_NEUTER},
              {FORMALITY_FORMAL, FORMALITY_INFORMAL},
              {CLUSIVITY_INCLUSIVE, CLUSIVITY_EXCLUSIVE},
              {PERSON_FIRST, PERSON_SECOND, PERSON_THIRD},
              {TENSE_PAST, TENSE_PRESENT, TENSE_FUTURE},
              {MOOD_INDICATIVE, MOOD_IMPERATIVE, MOOD_SUBJUNCTIVE}
          },
          {},
          true) 
{}

static std::u16string guessPluralForm(const std::u16string& token) {
    if (token.ends_with(u"ം")) {
        return token.substr(0, token.size() - 1) + u"ങ്ങൾ";
    }
    if (token.ends_with(u"ത്തി")) {
        return token.substr(0, token.size() - 3) + u"ത്തികൾ";
    }
    if (token.ends_with(u"ൻ")) {
        return token + u"മാർ";
    }
    if (token.ends_with(u"ി") || token.ends_with(u"ാ")) {
        return token + u"കൾ";
    }
    if (!token.empty() && token.back() != u'്') {
        return token + u"കൾ";
    }
    return token + u"ങ്ങൾ‍";
}

::inflection::dialog::DisplayValue* MlGrammarSynthesizer_MlDisplayFunction::getDisplayValue(
    const SemanticFeatureModel_DisplayData& displayData,
    const std::map<SemanticFeature, std::u16string>& constraints,
    bool enableInflectionGuess) const 
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr || displayValue->getDisplayString().empty()) {
        if (GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature) == u"pronoun") {
            std::u16string fallback = fallbackMalayalamPronoun(constraints);
            if (!fallback.empty()) {
                return new DisplayValue(fallback, constraints);
            }
        }
        return nullptr;
    }

    const std::u16string baseForm = displayValue->getDisplayString();
    const std::u16string posFeatureValue = GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature);
    const std::u16string numberFeatureValue = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature);
    const std::u16string caseValue = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
    const std::u16string moodVal = GrammarSynthesizerUtil::getFeatureValue(constraints, moodFeature);
    const std::u16string tenseVal = GrammarSynthesizerUtil::getFeatureValue(constraints, tenseFeature);

    int64_t wordGrammemes = 0;
    std::vector<std::u16string> constraintValues;

    auto addIfNotEmpty = [&](const SemanticFeature& feature) {
        const auto val = GrammarSynthesizerUtil::getFeatureValue(constraints, feature);
        if (!val.empty()) {
            constraintValues.push_back(val);
        }
    };

    addIfNotEmpty(caseFeature);
    addIfNotEmpty(numberFeature);

    if (posFeatureValue == GrammemeConstants::POS_PRONOUN()) {
        constraintValues.push_back(u"pos=pronoun");

        const auto personVal = GrammarSynthesizerUtil::getFeatureValue(constraints, personFeature);
        constraintValues.push_back(personVal.empty() ? PERSON_THIRD : personVal);

        const auto numberVal = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature);
        constraintValues.push_back(numberVal.empty() ? NUMBER_SINGULAR : numberVal);

        const auto genderVal = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);
        constraintValues.push_back(genderVal.empty() ? GENDER_MASCULINE : genderVal);

        const auto caseVal = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
        constraintValues.push_back(caseVal.empty() ? CASE_NOMINATIVE : caseVal);

        const auto formalityVal = GrammarSynthesizerUtil::getFeatureValue(constraints, formalityFeature);
        constraintValues.push_back(formalityVal.empty() ? FORMALITY_INFORMAL : formalityVal);

        const auto clusivityVal = GrammarSynthesizerUtil::getFeatureValue(constraints, clusivityFeature);
        constraintValues.push_back(clusivityVal.empty() ? CLUSIVITY_EXCLUSIVE : clusivityVal);

        const auto determinationVal = GrammarSynthesizerUtil::getFeatureValue(constraints, determinationFeature);
        constraintValues.push_back(determinationVal.empty() ? u"!dependent" : determinationVal);

        constraintValues.push_back(u"personal");
    }

    addIfNotEmpty(tenseFeature);
    addIfNotEmpty(moodFeature);

    if (!posFeatureValue.empty()) {
        constraintValues.push_back(posFeatureValue);
    }

    if (posFeatureValue == GrammemeConstants::POS_PRONOUN()) {
        addIfNotEmpty(genderFeature);
    }

    if (numberFeatureValue.empty()) {
        constraintValues.push_back(NUMBER_SINGULAR);
    }

    if (GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature).empty() &&
        posFeatureValue == GrammemeConstants::POS_PRONOUN()) {
        constraintValues.push_back(GENDER_MASCULINE);
    }

    const std::u16string determinationVal = GrammarSynthesizerUtil::getFeatureValue(constraints, determinationFeature);
    if (determinationVal.empty()) {
        constraintValues.push_back(u"!dependent");
    }

    auto inflectedOpt = dictionaryInflector.inflect(baseForm, wordGrammemes, constraintValues);

    if (inflectedOpt.has_value() && *inflectedOpt != baseForm) {
        std::u16string result = *inflectedOpt;

        if (std::find(constraintValues.begin(), constraintValues.end(), u"first") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"singular") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"nominative") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"exclusive") != constraintValues.end() &&
            (result == u"ഏൻ" || baseForm == u"ഏൻ")) {
            return new DisplayValue(u"ഞാൻ", constraints);
        }

        if (std::find(constraintValues.begin(), constraintValues.end(), u"first") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"plural") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"nominative") != constraintValues.end() &&
            std::find(constraintValues.begin(), constraintValues.end(), u"inclusive") != constraintValues.end() &&
            (result == u"നാം" || baseForm == u"നാം")) {
            return new DisplayValue(u"നമ്മൾ", constraints);
        }
        return new DisplayValue(result, constraints);
    }

    if (enableInflectionGuess && posFeatureValue != u"pronoun") {
        const bool isNoun = (posFeatureValue == u"noun" || posFeatureValue.empty());
        const bool isPluralTarget = (numberFeatureValue == NUMBER_PLURAL);

        if (isNoun && isPluralTarget) {
            std::u16string guessed = guessPluralForm(baseForm);
            if (guessed != baseForm) {
                return new DisplayValue(guessed, constraints);
            }
        }

        if (posFeatureValue == u"verb") {
            std::u16string stem = baseForm;
            static const std::vector<std::u16string> infinitiveSuffixes = {u"ക്കുക", u"കുക", u"വിക്കുക", u"പിക്കുക"};

            for (const auto& suffix : infinitiveSuffixes) {
                if (stem.size() >= suffix.size() &&
                    stem.compare(stem.size() - suffix.size(), suffix.size(), suffix) == 0) {
                    stem = stem.substr(0, stem.size() - suffix.size());
                    break;
                }
            }

            std::u16string conjugatedVerb;

            if (moodVal == MOOD_INDICATIVE) {
                if (tenseVal == TENSE_PRESENT) {
                    conjugatedVerb = stem + u"ിക്കുന്നു";
                } else if (tenseVal == TENSE_PAST) {
                    conjugatedVerb = stem + u"ിച്ചു";
                } else if (tenseVal == TENSE_FUTURE) {
                    conjugatedVerb = stem + u"ിക്കും";
                }
            } else if (moodVal == MOOD_IMPERATIVE) {
                conjugatedVerb = baseForm;
            } else if (moodVal == MOOD_SUBJUNCTIVE) {
                conjugatedVerb = stem + u"ക്കുമെന്ന്";
            }

            if (!conjugatedVerb.empty()) {
                return new DisplayValue(conjugatedVerb, constraints);
            }
        }

        if (!caseValue.empty()) {
            std::u16string result;

            if (caseValue == CASE_ACCUSATIVE) {
                if (baseForm.ends_with(u"ൻ")) {
                    result = baseForm.substr(0, baseForm.size() - 1) + u"നെ";
                } else if (baseForm.ends_with(u"ൾ")) {
                    result = baseForm.substr(0, baseForm.size() - 1) + u"ളെ";
                } else {
                    result = baseForm + u"യെ";
                }
            } else if (caseValue == CASE_DATIVE) {
                result = baseForm + u"ക്ക്";
            } else if (caseValue == CASE_GENITIVE) {
                result = baseForm + u"യുടെ";
            } else if (caseValue == CASE_LOCATIVE) {
                result = baseForm + u"യിൽ";
            } else if (caseValue == CASE_INSTRUMENTAL) {
                result = baseForm + u"യാൽ";
            } else if (caseValue == CASE_SOCIATIVE) {
                result = baseForm + u"ോടൊപ്പം";
            }

            if (!result.empty()) {
                return new DisplayValue(result, constraints);
            }
        }
    }

    if (posFeatureValue == u"pronoun") {
        std::u16string fallback = fallbackMalayalamPronoun(constraints);
        if (!fallback.empty()) {
            return new DisplayValue(fallback, constraints);
        }
    }

    return nullptr;
}

MlGrammarSynthesizer_MlDisplayFunction::~MlGrammarSynthesizer_MlDisplayFunction() = default;

}  // namespace inflection::grammar::synthesis
