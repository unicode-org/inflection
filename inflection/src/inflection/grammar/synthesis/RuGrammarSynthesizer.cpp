/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_AboutPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_InPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_RuDisplayFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ToPrepositionLookupFunction.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_WithPrepositionLookupFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <unicode/uchar.h>

namespace inflection::grammar::synthesis {

void RuGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.setDefaultDisplayFunction(new RuGrammarSynthesizer_RuDisplayFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(PREPOSITION_ABOUT, new RuGrammarSynthesizer_AboutPrepositionLookupFunction(featureModel, u""));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_ABOUT, new RuGrammarSynthesizer_AboutPrepositionLookupFunction(featureModel, PREPOSITION_ABOUT));
    featureModel.putDefaultFeatureFunctionByName(PREPOSITION_IN, new RuGrammarSynthesizer_InPrepositionLookupFunction(featureModel, u""));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_IN, new RuGrammarSynthesizer_InPrepositionLookupFunction(featureModel, PREPOSITION_IN));
    featureModel.putDefaultFeatureFunctionByName(PREPOSITION_WITH, new RuGrammarSynthesizer_WithPrepositionLookupFunction(featureModel, u""));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_WITH, new RuGrammarSynthesizer_WithPrepositionLookupFunction(featureModel, PREPOSITION_WITH));
    featureModel.putDefaultFeatureFunctionByName(PREPOSITION_TO, new RuGrammarSynthesizer_ToPrepositionLookupFunction(featureModel, u""));
    featureModel.putDefaultFeatureFunctionByName(WITH_PREPOSITION_TO, new RuGrammarSynthesizer_ToPrepositionLookupFunction(featureModel, PREPOSITION_TO));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::RUSSIAN(), {GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_NEUTER}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::ANIMACY, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::RUSSIAN(), {GrammemeConstants::ANIMACY_ANIMATE, GrammemeConstants::ANIMACY_INANIMATE}));
}

bool RuGrammarSynthesizer::startsWith2Consonant(const ::std::u16string& word, std::u16string_view firstLowercaseLetters)
{
    if (word.length() < 2) {
        return false;
    }
    if (firstLowercaseLetters.find(static_cast<char16_t>(u_tolower(word[0]))) != std::u16string::npos) {
        return !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(word[1]);
    }
    return false;
}

} // namespace inflection::grammar::synthesis
