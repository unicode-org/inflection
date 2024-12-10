/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 2/8/23.
//
#pragma once
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <string>
#include <vector>

class morphuntion::dialog::DeterminerAdpositionDetectionFunction {

private:
    const ::std::vector<::std::pair<const DefaultArticleLookupFunction*, const DefaultArticleLookupFunction::ArticleDisplayData>> articlesForLookupFunctions;

public:
    ::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue *> getArticleLookupFunction(::std::u16string_view displayString) const;
    ::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue *> detectAndStripArticlePrefix(::std::u16string &displayString) const;

public:
    explicit DeterminerAdpositionDetectionFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::vector<const char16_t *>& semanticFeatureNames);
};
