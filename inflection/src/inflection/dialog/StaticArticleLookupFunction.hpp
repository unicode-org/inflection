/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class inflection::dialog::StaticArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    ::std::u16string articlePrefix {  };
    ::std::u16string articleSpacedPrefix {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    StaticArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const char16_t* articlePrefix);
    StaticArticleLookupFunction(const StaticArticleLookupFunction&) = delete;
    StaticArticleLookupFunction& operator=(const StaticArticleLookupFunction&) = delete;
};
