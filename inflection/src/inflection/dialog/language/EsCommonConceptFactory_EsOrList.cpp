/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/EsCommonConceptFactory_EsOrList.hpp>

#include <inflection/dialog/language/EsCommonConceptFactory_EsAndList.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

EsCommonConceptFactory_EsOrList::EsCommonConceptFactory_EsOrList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(inflection::dialog::SpeakableString(u", "));
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
    static auto O_SOUND_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[oó]", USET_CASE_INSENSITIVE));
    return *npc(O_SOUND_);
}

inflection::dialog::SpeakableString EsCommonConceptFactory_EsOrList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), EsCommonConceptFactory_EsAndList::IMPORTANT_SET(), O_SOUND(), {})) {
        return inflection::dialog::SpeakableString(u" u ");
    }
    return inflection::dialog::SpeakableString(u" o ");
}

} // namespace inflection::dialog::language
