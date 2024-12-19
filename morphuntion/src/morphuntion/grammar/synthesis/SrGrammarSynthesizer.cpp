/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <morphuntion/grammar/synthesis/SrGrammarSynthesizer.hpp>

#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/SrGrammarSynthesizer_SrDisplayFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

void SrGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::SERBIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::SERBIAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));

    featureModel.setDefaultDisplayFunction(new SrGrammarSynthesizer_SrDisplayFunction(featureModel));
}

} // namespace morphuntion::grammar::synthesis
