/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/NbCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

NbCommonConceptFactory::NbCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

NbCommonConceptFactory::~NbCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
