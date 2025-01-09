/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/HeCommonConceptFactory_HeAndList.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>

namespace inflection::dialog::language {

HeCommonConceptFactory_HeAndList::HeCommonConceptFactory_HeAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(inflection::dialog::SpeakableString(u", "));
}

HeCommonConceptFactory_HeAndList::HeCommonConceptFactory_HeAndList(const HeCommonConceptFactory_HeAndList& other)
    : super(other)
{
}

HeCommonConceptFactory_HeAndList::~HeCommonConceptFactory_HeAndList()
{
}

HeCommonConceptFactory_HeAndList* HeCommonConceptFactory_HeAndList::clone() const
{
    return new HeCommonConceptFactory_HeAndList(*this);
}

inflection::dialog::SpeakableString HeCommonConceptFactory_HeAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (inflection::util::UnicodeSetUtils::containsSome(inflection::lang::StringFilterUtil::HEBREW_SCRIPT(), formattedLast.getPrint())) {
        return inflection::dialog::SpeakableString(u" ו");
    }
    return inflection::dialog::SpeakableString(u" ו-");
}

} // namespace inflection::dialog::language
