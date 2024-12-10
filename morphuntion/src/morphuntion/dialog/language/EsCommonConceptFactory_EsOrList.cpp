/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/EsCommonConceptFactory_EsOrList.hpp>

#include <morphuntion/dialog/language/EsCommonConceptFactory_EsAndList.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

EsCommonConceptFactory_EsOrList::EsCommonConceptFactory_EsOrList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(morphuntion::dialog::SpeakableString(u", "));
}

EsCommonConceptFactory_EsOrList::EsCommonConceptFactory_EsOrList(const EsCommonConceptFactory_EsOrList& other)
    : super(other)
{
}

EsCommonConceptFactory_EsOrList::~EsCommonConceptFactory_EsOrList()
{
}

EsCommonConceptFactory_EsOrList* EsCommonConceptFactory_EsOrList::clone() const
{
    return new EsCommonConceptFactory_EsOrList(*this);
}

static const icu4cxx::UnicodeSet& O_SOUND()
{
    static auto O_SOUND_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[oó]", USET_CASE_INSENSITIVE));
    return *npc(O_SOUND_);
}

morphuntion::dialog::SpeakableString EsCommonConceptFactory_EsOrList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), EsCommonConceptFactory_EsAndList::IMPORTANT_SET(), O_SOUND(), {})) {
        return morphuntion::dialog::SpeakableString(u" u ");
    }
    return morphuntion::dialog::SpeakableString(u" o ");
}

} // namespace morphuntion::dialog::language
