/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/EsCommonConceptFactory_EsAndList.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

EsCommonConceptFactory_EsAndList::EsCommonConceptFactory_EsAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(morphuntion::dialog::SpeakableString(u", "));
}

EsCommonConceptFactory_EsAndList::EsCommonConceptFactory_EsAndList(const EsCommonConceptFactory_EsAndList& other)
    : super(other)
{
}

EsCommonConceptFactory_EsAndList::~EsCommonConceptFactory_EsAndList()
{
}

EsCommonConceptFactory_EsAndList* EsCommonConceptFactory_EsAndList::clone() const
{
    return new EsCommonConceptFactory_EsAndList(*this);
}

const icu4cxx::UnicodeSet& EsCommonConceptFactory_EsAndList::IMPORTANT_SET()
{
    static auto IMPORTANT_SET_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[hH]]"));
    return *npc(IMPORTANT_SET_);
}

const icu4cxx::UnicodeSet& EsCommonConceptFactory_EsAndList::NON_SPANISH_VOWEL_SET()
{
    static auto NON_SPANISH_VOWEL_SET_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[aeiouAEIOU]]", USET_CASE_INSENSITIVE));
    return *npc(NON_SPANISH_VOWEL_SET_);
}

static const icu4cxx::UnicodeSet& I_SOUND()
{
    static auto I_SOUND_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[iyí]", USET_CASE_INSENSITIVE));
    return *npc(I_SOUND_);
}

morphuntion::dialog::SpeakableString EsCommonConceptFactory_EsAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), IMPORTANT_SET(), I_SOUND(), {&NON_SPANISH_VOWEL_SET()})) {
        return morphuntion::dialog::SpeakableString(u" e ");
    }
    return morphuntion::dialog::SpeakableString(u" y ");
}

} // namespace morphuntion::dialog::language
