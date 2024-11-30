/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer.hpp>

#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_AboutPrepositionLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_InPrepositionLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_RuDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_ToPrepositionLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_WithPrepositionLookupFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>

namespace morphuntion::grammar::synthesis {

void RuGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
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
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::RUSSIAN(), {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_NEUTER()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::ANIMACY, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::RUSSIAN(), {GrammemeConstants::ANIMACY_ANIMATE(), GrammemeConstants::ANIMACY_INANIMATE()}));
}

bool RuGrammarSynthesizer::startsWith2Consonant(const ::std::u16string& word, const ::icu4cxx::UnicodeSet& firstLetters)
{
    if (word.length() < 2) {
        return false;
    }
    auto firstChar = word[0];
    auto secondChar = word[1];
    if (firstLetters.contains(firstChar)) {
        return !::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(secondChar);
    }
    return false;
}

} // namespace morphuntion::grammar::synthesis
