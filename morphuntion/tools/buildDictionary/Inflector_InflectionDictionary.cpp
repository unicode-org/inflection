/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "Inflector_InflectionDictionary.hpp"

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>
#include <map>

static const ::std::set<::morphuntion::util::ULocale>& LEMMA_LOCALES() {
    static auto LEMMA_LOCALES_ = new ::std::set<::morphuntion::util::ULocale>({::morphuntion::util::LocaleUtils::FINNISH()});
    return *npc(LEMMA_LOCALES_);
}

Inflector_InflectionDictionary::Inflector_InflectionDictionary(const ::morphuntion::util::ULocale& locale)
{
    forLemmatization = (LEMMA_LOCALES().find(locale) != LEMMA_LOCALES().end());
}
