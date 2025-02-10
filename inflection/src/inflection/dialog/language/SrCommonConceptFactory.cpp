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
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

SrCommonConceptFactory::~SrCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
