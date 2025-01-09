/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/FrCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

FrCommonConceptFactory::FrCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
{
}

FrCommonConceptFactory::~FrCommonConceptFactory()
{
}

::inflection::dialog::SpeakableString FrCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    if (str.speakEqualsPrint()) {
        return ::inflection::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::inflection::dialog::SpeakableString(u"“" + str.getPrint() + u"”", str.getSpeak());
}

} // namespace inflection::dialog::language
