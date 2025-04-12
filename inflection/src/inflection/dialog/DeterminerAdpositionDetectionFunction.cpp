/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 2/8/23.
//

#include <inflection/dialog/DeterminerAdpositionDetectionFunction.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

static ::std::vector<::std::pair<const DefaultArticleLookupFunction*, const DefaultArticleLookupFunction::ArticleDisplayData>> convertToArticlesForLookupFunctions(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::vector<const char16_t *>& semanticFeatureNames) {
    ::std::vector<::std::pair<const DefaultArticleLookupFunction*, const DefaultArticleLookupFunction::ArticleDisplayData>> result;
    for (const auto semanticFeatureName : semanticFeatureNames) {
        const auto articleLookupFunction = npc(dynamic_cast<const DefaultArticleLookupFunction *>(model.getDefaultFeatureFunction(*npc(model.getFeature(semanticFeatureName)))));
        result.emplace_back(articleLookupFunction, articleLookupFunction->getPossibleArticles(model));
    }
    return result;
}

DeterminerAdpositionDetectionFunction::DeterminerAdpositionDetectionFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::vector<const char16_t *>& semanticFeatureNames)
    : articlesForLookupFunctions(convertToArticlesForLookupFunctions(model, semanticFeatureNames))
{
}

::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue *> DeterminerAdpositionDetectionFunction::getArticleLookupFunction(::std::u16string_view displayString) const{
    for (const auto& [articleLookupFunction, articleDisplayData] : articlesForLookupFunctions) {
        for (const auto &articleDisplayValue : articleDisplayData) {
            const auto &articleDisplayString = articleDisplayValue.first;
            if (articleDisplayString.length() > 0 && displayString.starts_with(articleDisplayString)) {
                if (articleLookupFunction->insertsSpace() && displayString.length() > articleDisplayString.length() && displayString.at(articleDisplayString.length()) != u' ') {
                    continue;
                }
                return {articleLookupFunction, &articleDisplayValue};
            }
        }
    }
    return {nullptr, nullptr};
}

::std::pair<const DefaultArticleLookupFunction *, const DefaultArticleLookupFunction::ArticleDisplayValue*> DeterminerAdpositionDetectionFunction::detectAndStripArticlePrefix(std::u16string &displayString) const {
    const auto [articleLookupFunction, articleDisplayValue] = getArticleLookupFunction(displayString);
    if (articleLookupFunction != nullptr && articleDisplayValue != nullptr) {
        int64_t articleDisplayStringLen = articleDisplayValue->first.length();
        if (articleLookupFunction->insertsSpace()) {
            articleDisplayStringLen += 1;
        }
        articleDisplayStringLen = ::std::min(articleDisplayStringLen, (int64_t) displayString.length());
        displayString = displayString.substr(articleDisplayStringLen);
    }
    return {articleLookupFunction, articleDisplayValue};
}

} // inflection::dialog
