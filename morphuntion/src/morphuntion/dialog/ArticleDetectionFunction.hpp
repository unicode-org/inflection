/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <string>
#include <vector>

class morphuntion::dialog::ArticleDetectionFunction
    : public virtual DefaultFeatureFunction
{
public:
    typedef DefaultFeatureFunction super;

private:
    ::morphuntion::util::ULocale locale;
    ::std::set<::std::u16string> definiteArticles;
    ::std::set<::std::u16string> indefiniteArticles;
    bool normalizeApostrophe { false };

public:
    SpeakableString* getFeatureValue(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const override;

public:
    ArticleDetectionFunction(const ::morphuntion::util::ULocale& locale, const ::std::set<::std::u16string>& definiteFeatures, const ::std::set<::std::u16string>& excludeDefiniteValues, const ::std::set<::std::u16string>& indefiniteFeatures, const ::std::set<::std::u16string>& excludeIndefiniteValues);
};
