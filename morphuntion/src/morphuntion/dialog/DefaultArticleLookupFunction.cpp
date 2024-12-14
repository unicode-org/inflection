/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>

#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog {

DefaultArticleLookupFunction::DefaultArticleLookupFunction(const SemanticFeatureModel& model, bool includeSemanticValue, bool insertSpace)
    : super()
    , includeSemanticValue(includeSemanticValue)
    , insertSpace(insertSpace)
    , speakFeature(model.getFeature(SemanticFeatureModel::SPEAK))
{
}

SpeakableString* DefaultArticleLookupFunction::createPreposition(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::u16string& article) const
{
    if (!includeSemanticValue) {
        return new SpeakableString(article);
    }
    const auto& displayString = displayValue.getDisplayString();
    auto speakableValue(displayString);
    auto speakableValuePtr = displayValue.getFeatureValue(*npc(speakFeature));
    if (speakableValuePtr != nullptr) {
        speakableValue = *npc(speakableValuePtr);
    }
    if (insertSpace && !displayString.empty() && !article.empty()) {
        return new SpeakableString(SpeakableString(article + u" ") + SpeakableString(displayString, speakableValue));
    }
    return new SpeakableString(SpeakableString(article) + SpeakableString(displayString, speakableValue));
}

SpeakableString* DefaultArticleLookupFunction::createPreposition(const SemanticFeatureModel_DisplayValue& displayValue, const SpeakableString& article) const
{
    if (!includeSemanticValue) {
        return new SpeakableString(article);
    }
    const auto& displayString = displayValue.getDisplayString();
    auto speakableValue(displayString);
    auto speakableValuePtr = displayValue.getFeatureValue(*npc(speakFeature));
    if (speakableValuePtr != nullptr) {
        speakableValue = *npc(speakableValuePtr);
    }
    if (insertSpace && !displayString.empty() && !article.isEmpty()) {
        return new SpeakableString(SpeakableString(article) + SpeakableString(u" ") + SpeakableString(displayString, speakableValue));
    }
    return new SpeakableString(SpeakableString(article) + SpeakableString(displayString, speakableValue));
}

SpeakableString* DefaultArticleLookupFunction::createPostposition(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::u16string& article) const
{
    if (!includeSemanticValue) {
        return new SpeakableString(article);
    }
    const auto& displayString = displayValue.getDisplayString();
    auto speakableValue(displayString);
    auto speakableValuePtr = displayValue.getFeatureValue(*npc(speakFeature));
    if (speakableValuePtr != nullptr) {
        speakableValue = *npc(speakableValuePtr);
    }
    if (insertSpace && !displayString.empty() && !article.empty()) {
        return new SpeakableString(SpeakableString(displayString, speakableValue) + SpeakableString(u" " + article));
    }
    return new SpeakableString(SpeakableString(displayString, speakableValue) + SpeakableString(article));
}

const ::std::u16string* DefaultArticleLookupFunction::getFeatureValue(const ::std::map<SemanticFeature, ::std::u16string>& constraints, const SemanticFeature& semanticFeature)
{
    auto result = constraints.find(semanticFeature);
    if (result != constraints.end()) {
        return &result->second;
    }
    return nullptr;
}

static DefaultArticleLookupFunction::ArticleDisplayData createArticleDisplayData(const SemanticFeatureModel& model, const ::morphuntion::lang::features::LanguageGrammarFeatures_GrammarFeatures& featureData) {
    DefaultArticleLookupFunction::ArticleDisplayData articleDisplayData;
    for (const auto &feature: featureData.getValues()) {
        if (feature.getValue().empty() || feature.getConstraints().empty()) {
            continue;
        }
        ::std::vector<::std::pair<const ::morphuntion::dialog::SemanticFeature* const, ::std::u16string>> displayValueConstraints;
        for (const auto& [constraintKey, constraintValue] : feature.getConstraints()) {
            const auto constraintFeature = npc(model.getFeature(constraintKey));
            displayValueConstraints.emplace_back(constraintFeature, constraintValue);
        }
        articleDisplayData.emplace_back(feature.getValue(), displayValueConstraints);
    }
    return articleDisplayData;
}

DefaultArticleLookupFunction::ArticleDisplayData DefaultArticleLookupFunction::getPossibleArticles(const SemanticFeatureModel& model) const {
    const auto semanticFeatureName = getDerivedSemanticName();
    if (semanticFeatureName == nullptr) {
        return {};
    }
    auto features = ::morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(model.getLocale());
    for (const auto& grammarFeatures : features.getFeatures()) {
        if (grammarFeatures.getName() == semanticFeatureName) {
            return createArticleDisplayData(model, grammarFeatures);
        }
    }
    return {};
}

bool DefaultArticleLookupFunction::insertsSpace() const {
    return insertSpace;
}

const char16_t *DefaultArticleLookupFunction::getDerivedSemanticName() const {
    return nullptr;
}


} // namespace morphuntion::dialog