/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/ArticleDetectionFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog {

static bool getFeatureValues(::std::set<::std::u16string>* articles, const std::vector<::morphuntion::lang::features::LanguageGrammarFeatures_Feature>& featureValues, const ::std::set<::std::u16string>& excludeValues)
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

ArticleDetectionFunction::ArticleDetectionFunction(const ::morphuntion::util::ULocale& locale, const ::std::set<::std::u16string>& definiteFeatures, const ::std::set<::std::u16string>& excludeDefiniteValues, const ::std::set<::std::u16string>& indefiniteFeatures, const ::std::set<::std::u16string>& excludeIndefiniteValues)
    : super()
    , locale(locale)
{
    auto features = ::morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale.getLanguage());
    for (const auto& feature : features.getFeatures()) {
        std::vector<::morphuntion::lang::features::LanguageGrammarFeatures_Feature> values(feature.getValues());
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

SpeakableString* ArticleDetectionFunction::getFeatureValue(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (displayString.length() < 2) {
        return nullptr;
    }
    std::u16string lowercaseString;
    morphuntion::util::StringViewUtils::lowercase(&lowercaseString, displayString, locale);
    if (normalizeApostrophe) {
        auto apostropheIndex = lowercaseString.find(u'\'');
        if (apostropheIndex != std::u16string::npos) {
            lowercaseString[apostropheIndex] = u'’';
        }
    }

    bool isDefinite = false;
    bool isIndefinite = false;
    for (const auto& definiteArticle : definiteArticles) {
        if (morphuntion::util::StringViewUtils::startsWith(lowercaseString, definiteArticle)) {
            isDefinite = true;
            break;
        }
    }
    for (const auto& indefiniteArticle : indefiniteArticles) {
        if (morphuntion::util::StringViewUtils::startsWith(lowercaseString, indefiniteArticle)) {
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

} // namespace morphuntion::dialog
