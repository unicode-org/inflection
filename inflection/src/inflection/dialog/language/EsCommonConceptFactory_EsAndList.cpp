/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/EsCommonConceptFactory_EsAndList.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

EsCommonConceptFactory_EsAndList::EsCommonConceptFactory_EsAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(inflection::dialog::SpeakableString(u", "));
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
    static auto IMPORTANT_SET_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[hH]]"));
    return *npc(IMPORTANT_SET_);
}

const icu4cxx::UnicodeSet& EsCommonConceptFactory_EsAndList::NON_SPANISH_VOWEL_SET()
{
    static auto NON_SPANISH_VOWEL_SET_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:][:Decimal_Number:]]-[aeiouAEIOU]]", USET_CASE_INSENSITIVE));
    return *npc(NON_SPANISH_VOWEL_SET_);
}

static const icu4cxx::UnicodeSet& I_SOUND()
{
    static auto I_SOUND_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[iyí]", USET_CASE_INSENSITIVE));
    return *npc(I_SOUND_);
}

inflection::dialog::SpeakableString EsCommonConceptFactory_EsAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& formattedLast) const
{
    if (::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(formattedLast.getPrint(), IMPORTANT_SET(), I_SOUND(), {&NON_SPANISH_VOWEL_SET()})) {
        return inflection::dialog::SpeakableString(u" e ");
    }
    return inflection::dialog::SpeakableString(u" y ");
}

} // namespace inflection::dialog::language
