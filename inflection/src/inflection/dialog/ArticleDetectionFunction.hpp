/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/util/ULocale.hpp>
#include <string>
#include <set>

class inflection::dialog::ArticleDetectionFunction
    : public virtual DefaultFeatureFunction
{
public:
    typedef DefaultFeatureFunction super;

private:
    ::inflection::util::ULocale locale;
    ::std::set<::std::u16string, std::less<>> definiteArticles {  };
    ::std::set<::std::u16string, std::less<>> indefiniteArticles {  };
    bool normalizeApostrophe { false };

public:
    SpeakableString* getFeatureValue(const DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const override;

public:
    ArticleDetectionFunction(const ::inflection::util::ULocale& locale, const ::std::set<::std::u16string, std::less<>>& definiteFeatures, const ::std::set<::std::u16string, std::less<>>& excludeDefiniteValues, const ::std::set<::std::u16string, std::less<>>& indefiniteFeatures, const ::std::set<::std::u16string, std::less<>>& excludeIndefiniteValues);
    ArticleDetectionFunction(const ArticleDetectionFunction&) = delete;
    ArticleDetectionFunction& operator=(const ArticleDetectionFunction&) = delete;
};
