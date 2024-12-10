/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::ZhCommonConceptFactory
    : public morphuntion::dialog::CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:

protected: /* package */
    ::morphuntion::dialog::SpeakableString quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const override;

public:
    explicit ZhCommonConceptFactory(const ::morphuntion::util::ULocale& language);
};
