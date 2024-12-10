/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class morphuntion::dialog::StaticArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };
    ::std::u16string articlePrefix {  };
    ::std::u16string articleSpacedPrefix {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    StaticArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const char16_t* articlePrefix);
};
