/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/EsCommonConceptFactory.hpp>

#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/dialog/language/EsCommonConceptFactory_EsAndList.hpp>
#include <morphuntion/dialog/language/EsCommonConceptFactory_EsOrList.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

EsCommonConceptFactory::EsCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
{
}

EsCommonConceptFactory::~EsCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* EsCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new EsCommonConceptFactory_EsOrList(getSemanticFeatureModel(), concepts);
}

morphuntion::dialog::SemanticConceptList* EsCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new EsCommonConceptFactory_EsAndList(getSemanticFeatureModel(), concepts);
}

} // namespace morphuntion::dialog::language
