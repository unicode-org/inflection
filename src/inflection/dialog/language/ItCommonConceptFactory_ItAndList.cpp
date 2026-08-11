/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/ItCommonConceptFactory_ItAndList.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

ItCommonConceptFactory_ItAndList::ItCommonConceptFactory_ItAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(inflection::dialog::SpeakableString(u", "));
}

ItCommonConceptFactory_ItAndList::ItCommonConceptFactory_ItAndList(const ItCommonConceptFactory_ItAndList& other)
    : super(other)
{
}

ItCommonConceptFactory_ItAndList::~ItCommonConceptFactory_ItAndList()
{
}

ItCommonConceptFactory_ItAndList* ItCommonConceptFactory_ItAndList::clone() const
{
    return new ItCommonConceptFactory_ItAndList(*this);
}

const icu4cxx::UnicodeSet& ItCommonConceptFactory_ItAndList::IMPORTANT_SET()
{
    static auto IMPORTANT_SET_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[hH]]"));
    return *npc(IMPORTANT_SET_);
}

static const icu4cxx::UnicodeSet& E_SOUND()
{
    static auto I_SOUND_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[eèé]", USET_CASE_INSENSITIVE));
    return *npc(I_SOUND_);
}

inflection::dialog::SpeakableString ItCommonConceptFactory_ItAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), IMPORTANT_SET(), E_SOUND(), {})) {
        return inflection::dialog::SpeakableString(u" ed ");
    }
    return inflection::dialog::SpeakableString(u" e ");
}

} // namespace inflection::dialog::language
