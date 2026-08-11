/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeatureModel.hpp>

#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerFactory.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>

namespace inflection::dialog {

SemanticFeatureModel::SemanticFeatureModel(const ::inflection::util::ULocale& locale)
    : SemanticFeatureModel(locale, {})
{
}

SemanticFeatureModel::SemanticFeatureModel(const ::inflection::util::ULocale& locale, const ::std::map<SemanticValue, SemanticFeatureModel_DisplayData>& semanticValueMap)
    : super()
    , semanticValueMap(semanticValueMap)
    , locale(locale)
{
    auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
    for (const auto&[catName, category] : features.getCategories()) {
        auto [featureItr, inserted] = featureMap.emplace(category.getName(), SemanticFeature(category.getName(), SemanticFeature::Type::BoundedValue, category.getValues(), category.isUniqueValues()));
        if (featureItr->second.isAliased()) {
            for (const auto& val : category.getValues()) {
                featuresAliases.emplace(val, ::std::make_pair(&featureItr->second, val));
            }
        }
    }
    for (const auto& grammarFeatures : features.getFeatures()) {
        ::std::set<::std::u16string> knownValues;
        for (const auto& feature : grammarFeatures.getValues()) {
            knownValues.insert(feature.getValue());
        }
        featureMap.emplace(grammarFeatures.getName(), SemanticFeature(grammarFeatures.getName(), !knownValues.empty() ? SemanticFeature::Type::BoundedValue : SemanticFeature::Type::UnboundedValue, knownValues, false));
    }
    ::inflection::grammar::synthesis::GrammarSynthesizerFactory::addSemanticFeatures(locale, *this);
}

SemanticFeatureModel::~SemanticFeatureModel()
{
    for (const auto& defaultFeatureFunction : defaultFeatureFunctions) {
        delete defaultFeatureFunction.second;
    }
}

::std::set<SemanticValue> SemanticFeatureModel::getKnownSemanticValues() const
{
    ::std::set<SemanticValue> result;
    for (const auto& displayDataPair : semanticValueMap) {
        result.insert(displayDataPair.first);
    }
    return result;
}

const SemanticFeatureModel_DisplayData* SemanticFeatureModel::getDisplayData(const SemanticValue& semantic) const
{
    auto result = semanticValueMap.find(semantic);
    if (result == semanticValueMap.end()) {
        return nullptr;
    }
    return &result->second;
}

const SemanticFeature* SemanticFeatureModel::getFeature(::std::u16string_view name) const
{
    auto result = featureMap.find(name);
    if (result == featureMap.end()) {
        return nullptr;
    }
    return &result->second;
}

::std::pair<SemanticFeature*, ::std::u16string> SemanticFeatureModel::getFeatureAlias(::std::u16string_view name) const
{
    auto result = featuresAliases.find(name);
    if (result == featuresAliases.end()) {
        return {};
    }
    return result->second;
}

const ::inflection::util::ULocale& SemanticFeatureModel::getLocale() const
{
    return locale;
}

const DefaultFeatureFunction* SemanticFeatureModel::getDefaultFeatureFunction(const SemanticFeature& feature) const
{
    auto result = defaultFeatureFunctions.find(feature);
    if (result == defaultFeatureFunctions.end()) {
        return nullptr;
    }
    return result->second;
}

void SemanticFeatureModel::putDefaultFeatureFunction(const SemanticFeature& feature, DefaultFeatureFunction* function)
{
    auto previousResult = defaultFeatureFunctions.find(feature);
    if (previousResult != defaultFeatureFunctions.end()) {
        delete previousResult->second;
        previousResult->second = function;
    }
    else {
        defaultFeatureFunctions[feature] = function;
    }
}

void SemanticFeatureModel::putDefaultFeatureFunctionByName(::std::u16string_view feature, DefaultFeatureFunction* function)
{
    auto semanticFeature = getFeature(feature);
    if (semanticFeature == nullptr) {
        throw ::inflection::exception::IllegalArgumentException(std::u16string(feature) + u" is not a valid SemanticFeature for language " + locale.toString());
    }
    putDefaultFeatureFunction(*semanticFeature, function);
}

const DefaultDisplayFunction* SemanticFeatureModel::getDefaultDisplayFunction() const
{
    return defaultDisplayFunction.get();
}

void SemanticFeatureModel::setDefaultDisplayFunction(DefaultDisplayFunction* defaultDisplayFunction)
{
    this->defaultDisplayFunction.reset(defaultDisplayFunction);
}

} // namespace inflection::dialog
