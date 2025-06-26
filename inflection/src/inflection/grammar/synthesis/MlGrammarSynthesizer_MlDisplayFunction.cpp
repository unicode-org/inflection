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
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

using dialog::SemanticFeature;
using dialog::SemanticFeatureModel_DisplayData;
using dialog::DisplayValue;

// Malayalam-specific grammemes
static constexpr auto CASE_NOMINATIVE = u"nominative";
static constexpr auto CASE_ACCUSATIVE = u"accusative";
static constexpr auto CASE_DATIVE     = u"dative";
static constexpr auto CASE_GENITIVE   = u"genitive";
static constexpr auto CASE_INSTRUMENTAL   = u"instrumental";
static constexpr auto CASE_LOCATIVE   = u"locative";
static constexpr auto CASE_SOCIATIVE   = u"sociative";
static constexpr auto NUMBER_SINGULAR = u"singular";
static constexpr auto NUMBER_PLURAL   = u"plural";
static constexpr auto GENDER_MASCULINE = u"masculine";
static constexpr auto GENDER_FEMININE  = u"feminine";
static constexpr auto GENDER_NEUTER    = u"neuter";
static constexpr auto FORMALITY_FORMAL   = u"formal";
static constexpr auto FORMALITY_INFORMAL = u"informal";
static constexpr auto CLUSIVITY_INCLUSIVE   = u"inclusive";
static constexpr auto CLUSIVITY_EXCLUSIVE = u"exclusive";
static constexpr auto PERSON_FIRST = u"first";
static constexpr auto PERSON_SECOND = u"second";
static constexpr auto PERSON_THIRD  = u"third";
static constexpr auto TENSE_PAST    = u"past";
static constexpr auto TENSE_PRESENT = u"present";
static constexpr auto TENSE_FUTURE  = u"future";
static constexpr auto MOOD_INDICATIVE   = u"indicative";
static constexpr auto MOOD_IMPERATIVE   = u"imperative";
static constexpr auto MOOD_SUBJUNCTIVE  = u"subjunctive";
// static constexpr auto POS_PRONOUN = u"pos_pronoun";

MlGrammarSynthesizer_MlDisplayFunction::MlGrammarSynthesizer_MlDisplayFunction(
    const ::inflection::dialog::SemanticFeatureModel& model)
    : caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , formalityFeature(*npc(model.getFeature(u"formality")))    
    , clusivityFeature(*npc(model.getFeature(u"clusivity")))
    , personFeature(*npc(model.getFeature(GrammemeConstants::PERSON)))
    , tenseFeature(*npc(model.getFeature(u"tense")))
    , moodFeature(*npc(model.getFeature(u"mood")))

{
    // Constructor initializes feature references
}

::inflection::dialog::DisplayValue* MlGrammarSynthesizer_MlDisplayFunction::getDisplayValue(
    const SemanticFeatureModel_DisplayData& displayData,
    const std::map<SemanticFeature, std::u16string>& constraints,
    bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }

    const std::u16string posFeatureValue = GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature);
    const std::u16string numberFeatureValue = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature);

    std::u16string baseForm = displayValue->getDisplayString();
    
    int64_t wordGrammemes = 0;

    std::vector<std::u16string> constraintValues;
    auto addIfNotEmpty = [&](const SemanticFeature& feature) {
        auto val = GrammarSynthesizerUtil::getFeatureValue(constraints, feature);
        if (!val.empty()) {
            constraintValues.push_back(val);
        }
    };

    // Add case, number always
    addIfNotEmpty(caseFeature);
    addIfNotEmpty(numberFeature);

    // Add gender only if pos is pronoun
    if (posFeatureValue == u"pronoun") {
        addIfNotEmpty(genderFeature);
    }

    addIfNotEmpty(formalityFeature);
    addIfNotEmpty(clusivityFeature);
    addIfNotEmpty(personFeature);
    addIfNotEmpty(tenseFeature);
    addIfNotEmpty(moodFeature);

    if (!posFeatureValue.empty()) {
        constraintValues.push_back(posFeatureValue);
    }

    if (posFeatureValue == u"adjective") {
        return new DisplayValue(baseForm, constraints);
    }

    static dialog::DictionaryLookupInflector dictionaryInflector(
        util::LocaleUtils::MALAYALAM(),
        {
            {CASE_NOMINATIVE, CASE_ACCUSATIVE, CASE_DATIVE, CASE_GENITIVE, CASE_LOCATIVE, CASE_INSTRUMENTAL, CASE_SOCIATIVE},
            {NUMBER_SINGULAR, NUMBER_PLURAL},
            {GENDER_MASCULINE, GENDER_FEMININE, GENDER_NEUTER},
            {FORMALITY_FORMAL, FORMALITY_INFORMAL},
            {CLUSIVITY_INCLUSIVE, CLUSIVITY_EXCLUSIVE},
            {PERSON_FIRST, PERSON_SECOND, PERSON_THIRD},
            {TENSE_PAST, TENSE_PRESENT, TENSE_FUTURE},
            {MOOD_INDICATIVE, MOOD_IMPERATIVE, MOOD_SUBJUNCTIVE}
        },
        {},
        true
    );

    auto inflectedOpt = dictionaryInflector.inflect(baseForm, wordGrammemes, constraintValues);
    if (inflectedOpt.has_value()) {
        return new DisplayValue(*inflectedOpt, constraints);
    }

    if (!inflectedOpt.has_value() && enableInflectionGuess) {
    const bool isNoun = (posFeatureValue == u"noun");
    const bool isPluralTarget = (numberFeatureValue == NUMBER_PLURAL);

    if (isNoun && isPluralTarget) {
        const std::u16string token = displayValue->getDisplayString();

        std::u16string guessed = token;
        if (!token.empty() && token.back() != u'്') {
            guessed += u"കൾ";
        } else {
            guessed += u"ങ്ങൾ‍";
        }

        if (guessed != token) {
            return new DisplayValue(guessed, constraints);
        }
    }
}

return nullptr;
}

MlGrammarSynthesizer_MlDisplayFunction::~MlGrammarSynthesizer_MlDisplayFunction() = default;

} // namespace inflection::grammar::synthesis
