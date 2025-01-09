/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "Inflector_InflectionDictionary.hpp"

#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <map>

static const ::std::set<::inflection::util::ULocale>& LEMMA_LOCALES() {
    static auto LEMMA_LOCALES_ = new ::std::set<::inflection::util::ULocale>({::inflection::util::LocaleUtils::FINNISH()});
    return *npc(LEMMA_LOCALES_);
}

Inflector_InflectionDictionary::Inflector_InflectionDictionary(const ::inflection::util::ULocale& locale)
{
    forLemmatization = (LEMMA_LOCALES().find(locale) != LEMMA_LOCALES().end());
}
