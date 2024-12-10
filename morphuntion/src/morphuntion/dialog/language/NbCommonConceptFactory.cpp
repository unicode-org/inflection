/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/NbCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

NbCommonConceptFactory::NbCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"},
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

NbCommonConceptFactory::~NbCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
