/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/EsCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/language/EsCommonConceptFactory_EsAndList.hpp>
#include <inflection/dialog/language/EsCommonConceptFactory_EsOrList.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

EsCommonConceptFactory::EsCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
{
}

EsCommonConceptFactory::~EsCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* EsCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new EsCommonConceptFactory_EsOrList(getSemanticFeatureModel(), concepts);
}

inflection::dialog::SemanticConceptList* EsCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new EsCommonConceptFactory_EsAndList(getSemanticFeatureModel(), concepts);
}

} // namespace inflection::dialog::language
