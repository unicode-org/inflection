/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <morphuntion/dialog/language/SrCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

SrCommonConceptFactory::SrCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"},
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

SrCommonConceptFactory::~SrCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
