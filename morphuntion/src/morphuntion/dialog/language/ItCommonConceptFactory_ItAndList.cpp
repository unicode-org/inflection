/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/ItCommonConceptFactory_ItAndList.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

ItCommonConceptFactory_ItAndList::ItCommonConceptFactory_ItAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(morphuntion::dialog::SpeakableString(u", "));
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
    static auto IMPORTANT_SET_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[hH]]"));
    return *npc(IMPORTANT_SET_);
}

static const icu4cxx::UnicodeSet& E_SOUND()
{
    static auto I_SOUND_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[eèé]", USET_CASE_INSENSITIVE));
    return *npc(I_SOUND_);
}

morphuntion::dialog::SpeakableString ItCommonConceptFactory_ItAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), IMPORTANT_SET(), E_SOUND(), {})) {
        return morphuntion::dialog::SpeakableString(u" ed ");
    }
    return morphuntion::dialog::SpeakableString(u" e ");
}

} // namespace morphuntion::dialog::language
