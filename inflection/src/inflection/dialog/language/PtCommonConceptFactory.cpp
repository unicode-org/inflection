/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/PtCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

PtCommonConceptFactory::PtCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
{
}

PtCommonConceptFactory::~PtCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
