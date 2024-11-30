/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer.hpp>

#include <morphuntion/dialog/ArticleDetectionFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_CaseLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_CountLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_EnDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>

namespace morphuntion::grammar::synthesis {

void EnGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new EnGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new EnGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new EnGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new EnGrammarSynthesizer_CaseLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new morphuntion::dialog::ArticleDetectionFunction(morphuntion::util::LocaleUtils::ENGLISH(),
        {ARTICLE_DEFINITE}, {}, {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new EnGrammarSynthesizer_EnDisplayFunction(featureModel));
}

} // namespace morphuntion::grammar::synthesis
