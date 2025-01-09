/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 2/8/23.
//
#pragma once
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <string>
#include <vector>

class inflection::dialog::DeterminerAdpositionDetectionFunction {

private:
    const ::std::vector<::std::pair<const DefaultArticleLookupFunction*, const DefaultArticleLookupFunction::ArticleDisplayData>> articlesForLookupFunctions;

public:
    ::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue *> getArticleLookupFunction(::std::u16string_view displayString) const;
    ::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue *> detectAndStripArticlePrefix(::std::u16string &displayString) const;

public:
    explicit DeterminerAdpositionDetectionFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::vector<const char16_t *>& semanticFeatureNames);
};
