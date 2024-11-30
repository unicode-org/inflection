/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/HeCommonConceptFactory_HeAndList.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>

namespace morphuntion::dialog::language {

HeCommonConceptFactory_HeAndList::HeCommonConceptFactory_HeAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(morphuntion::dialog::SpeakableString(u", "));
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

morphuntion::dialog::SpeakableString HeCommonConceptFactory_HeAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (morphuntion::util::UnicodeSetUtils::containsSome(morphuntion::lang::StringFilterUtil::HEBREW_SCRIPT(), formattedLast.getPrint())) {
        return morphuntion::dialog::SpeakableString(u" ו");
    }
    return morphuntion::dialog::SpeakableString(u" ו-");
}

} // namespace morphuntion::dialog::language
