/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/FrCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

FrCommonConceptFactory::FrCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
{
}

FrCommonConceptFactory::~FrCommonConceptFactory()
{
}

::morphuntion::dialog::SpeakableString FrCommonConceptFactory::quote(const ::morphuntion::dialog::SpeakableString& str) const
{
    if (str.speakEqualsPrint()) {
        return ::morphuntion::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::morphuntion::dialog::SpeakableString(u"“" + str.getPrint() + u"”", str.getSpeak());
}

} // namespace morphuntion::dialog::language
