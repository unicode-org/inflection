/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EnGrammarSynthesizer.hpp>

#include <inflection/dialog/ArticleDetectionFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_CaseLookupFunction.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_EnDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void EnGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new EnGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new EnGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new EnGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new EnGrammarSynthesizer_CaseLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new inflection::dialog::ArticleDetectionFunction(inflection::util::LocaleUtils::ENGLISH(),
        {ARTICLE_DEFINITE}, {}, {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new EnGrammarSynthesizer_EnDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis
