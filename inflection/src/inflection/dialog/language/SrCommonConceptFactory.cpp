/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <inflection/dialog/language/SrCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::dialog::language {

    SrCommonConceptFactory::SrCommonConceptFactory(const ::inflection::util::ULocale &language)
        : super(language),
        semanticFeatureCase(*npc(semanticFeatureModel.getFeature(GrammemeConstants::CASE))),
        semanticFeatureGender(*npc(semanticFeatureModel.getFeature(GrammemeConstants::GENDER))),
        semanticFeatureNumber(*npc(semanticFeatureModel.getFeature(GrammemeConstants::NUMBER))),
        semanticFeatureAnimacy(*npc(semanticFeatureModel.getFeature(GrammemeConstants::ANIMACY)))
    {
}

SrCommonConceptFactory::~SrCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
