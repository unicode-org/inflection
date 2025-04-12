/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/ArticleDetectionFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

static bool getFeatureValues(::std::set<::std::u16string>* articles, const std::vector<::inflection::lang::features::LanguageGrammarFeatures_Feature>& featureValues, const ::std::set<::std::u16string>& excludeValues)
{
    bool hasTrailingSpace = false;
    bool normalizeApostrophe = false;
    for (const auto& featureValue : featureValues) {
        std::u16string featureValueStr(featureValue.getValue());
        if (featureValueStr.empty()) {
            continue;
        }
        if (featureValueStr.at(featureValueStr.length() - 1) == u' ') {
            hasTrailingSpace = true;
            break;
        }
    }
    for (const auto& featureValue : featureValues) {
        std::u16string featureValueStr(featureValue.getValue());
        if (featureValueStr.empty() || excludeValues.find(featureValueStr) != excludeValues.end()) {
            continue;
        }
        normalizeApostrophe = normalizeApostrophe || featureValueStr.find(u'’') != std::u16string::npos;
        npc(articles)->insert(hasTrailingSpace ? featureValueStr : featureValueStr + u' ');
    }
    return normalizeApostrophe;
}

ArticleDetectionFunction::ArticleDetectionFunction(const ::inflection::util::ULocale& locale, const ::std::set<::std::u16string>& definiteFeatures, const ::std::set<::std::u16string>& excludeDefiniteValues, const ::std::set<::std::u16string>& indefiniteFeatures, const ::std::set<::std::u16string>& excludeIndefiniteValues)
    : super()
    , locale(locale)
{
    auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale.getLanguage());
    for (const auto& feature : features.getFeatures()) {
        std::vector<::inflection::lang::features::LanguageGrammarFeatures_Feature> values(feature.getValues());
        if (values.empty()) {
            continue;
        }
        if (definiteFeatures.find(feature.getName()) != definiteFeatures.end()) {
            normalizeApostrophe = getFeatureValues(&definiteArticles, values, excludeDefiniteValues) || normalizeApostrophe;
        }
        if (indefiniteFeatures.find(feature.getName()) != indefiniteFeatures.end()) {
            normalizeApostrophe = getFeatureValues(&indefiniteArticles, values, excludeIndefiniteValues) || normalizeApostrophe;
        }
    }
}

SpeakableString* ArticleDetectionFunction::getFeatureValue(const DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (displayString.length() < 2) {
        return nullptr;
    }
    std::u16string lowercaseString;
    inflection::util::StringViewUtils::lowercase(&lowercaseString, displayString, locale);
    if (normalizeApostrophe) {
        auto apostropheIndex = lowercaseString.find(u'\'');
        if (apostropheIndex != std::u16string::npos) {
            lowercaseString[apostropheIndex] = u'’';
        }
    }

    bool isDefinite = false;
    bool isIndefinite = false;
    for (const auto& definiteArticle : definiteArticles) {
        if (lowercaseString.starts_with(definiteArticle)) {
            isDefinite = true;
            break;
        }
    }
    for (const auto& indefiniteArticle : indefiniteArticles) {
        if (lowercaseString.starts_with(indefiniteArticle)) {
            isIndefinite = true;
            break;
        }
    }
    if (isDefinite && !isIndefinite) {
        return new SpeakableString(u"definite");
    }
    if (!isDefinite && isIndefinite) {
        return new SpeakableString(u"indefinite");
    }
    return nullptr;
}

} // namespace inflection::dialog
