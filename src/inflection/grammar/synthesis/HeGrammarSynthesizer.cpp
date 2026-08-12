/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_DefinitenessLookupFunction.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_HeDisplayFunction.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void HeGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.setDefaultDisplayFunction(new HeGrammarSynthesizer_HeDisplayFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::grammar::GrammemeLookupFunction(
        ::inflection::util::LocaleUtils::HEBREW(),
        {GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_MASCULINE, },
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_DETERMINER, GrammemeConstants::POS_VERB},
        GrammemeConstants::GENDER_MASCULINE, true,
        [](const ::std::u16string& word) -> ::std::u16string {
            if (word.ends_with(u"ת") || word.ends_with(u"ה")) {
                return GrammemeConstants::GENDER_FEMININE;
            }
            return {};
        }));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::HEBREW(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL, GrammemeConstants::NUMBER_DUAL}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new HeGrammarSynthesizer_DefinitenessLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(WITH_CONDITIONAL_HYPHEN, new HeGrammarSynthesizer_WithConditionalHyphen());
}

} // namespace inflection::grammar::synthesis
