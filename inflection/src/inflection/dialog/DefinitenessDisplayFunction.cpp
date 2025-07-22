/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>

#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <map>
#include <memory>

namespace inflection::dialog {

DefinitenessDisplayFunction::DefinitenessDisplayFunction(const SemanticFeatureModel& model,
                                                          const DefaultFeatureFunction* definiteFeatureFunction, std::u16string_view definiteFeatureArticlesName,
                                                          const DefaultFeatureFunction* indefiniteFeatureFunction, std::u16string_view indefiniteFeatureArticlesName)
    : super()
    , definiteFeature(model.getFeature(inflection::grammar::synthesis::GrammemeConstants::DEFINITENESS))
    , speakFeature(model.getFeature(SemanticFeatureModel::SPEAK))
    , definiteFeatureFunction(definiteFeatureFunction)
    , indefiniteFeatureFunction(indefiniteFeatureFunction)
    , definiteArticles(getArticles(model.getLocale(), definiteFeatureArticlesName))
    , indefiniteArticles(getArticles(model.getLocale(), indefiniteFeatureArticlesName))
{
}

DefinitenessDisplayFunction::DefinitenessDisplayFunction(const SemanticFeatureModel& model, const ::std::u16string& definiteFeatureName, std::u16string_view definiteFeatureArticlesName, const ::std::u16string& indefiniteFeatureName, std::u16string_view indefiniteFeatureArticlesName)
    : DefinitenessDisplayFunction(model,
                                  model.getDefaultFeatureFunction(*npc(model.getFeature(definiteFeatureName))),
                                  definiteFeatureArticlesName,
                                  model.getDefaultFeatureFunction(*npc(model.getFeature(indefiniteFeatureName))),
                                  indefiniteFeatureArticlesName)
{
}

DefinitenessDisplayFunction::~DefinitenessDisplayFunction()
{
}

std::set<std::u16string> DefinitenessDisplayFunction::getArticles(const inflection::util::ULocale& locale, std::u16string_view featureName)
{
    std::set<std::u16string> result;
    for (const auto& feature : inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale).getFeatures()) {
        if (feature.getName() == featureName) {
            bool hasWhitespaceSuffix = false;
            for (const auto& featureValue : feature.getValues()) {
                if (!featureValue.getValue().empty() && featureValue.getValue().back() == u' ') {
                    hasWhitespaceSuffix = true;
                    break;
                }
            }
            for (const auto& featureValue : feature.getValues()) {
                result.emplace(featureValue.getValue() + (hasWhitespaceSuffix ? u"" : u" "));
            }
            break;
        }
    }
    return result;
}

int32_t DefinitenessDisplayFunction::getArticlePrefixLength(DisplayValue* originalDisplayValue, const std::set<std::u16string>& articlesToRemove)
{
    std::u16string lowerCasedValue;
    // Turkish doesn't have articles. So the language is not important in the following lower casing call.
    inflection::util::StringViewUtils::lowercase(&lowerCasedValue, npc(originalDisplayValue)->getDisplayString(), ::inflection::util::LocaleUtils::ROOT());
    for (const auto& featureValue : articlesToRemove) {
        if (featureValue.length() < lowerCasedValue.length() && lowerCasedValue.compare(0, featureValue.length(), featureValue) == 0) {
            return int32_t(featureValue.length());
        }
    }
    return -1;
}

DisplayValue*
DefinitenessDisplayFunction::replaceDisplayValue(DisplayValue* originalDisplayValue, const SpeakableString& string) const
{
    ::std::unique_ptr<DisplayValue> value(npc(originalDisplayValue));
    auto displayValueConstraints(value->getConstraintMap());
    if (!string.speakEqualsPrint()) {
        displayValueConstraints[*npc(speakFeature)] = string.getSpeak();
    }
    value.reset(new DisplayValue(string.getPrint(), displayValueConstraints));
    return value.release();
}

DisplayValue*
DefinitenessDisplayFunction::addDefiniteness(DisplayValue* displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const
{
    if (displayValue != nullptr) {
        auto definiteness = constraints.find(*npc(definiteFeature));
        if (definiteness != constraints.end()) {
            std::u16string displayString;
            int32_t definitePrefixLength = 0;
            int32_t indefinitePrefixLength = 0;
            if ((definitePrefixLength = getArticlePrefixLength(displayValue, definiteArticles)) > 0) {
                displayString = npc(displayValue)->getDisplayString().substr(definitePrefixLength);
            }
            else if ((indefinitePrefixLength = getArticlePrefixLength(displayValue, indefiniteArticles)) > 0) {
                displayString = npc(displayValue)->getDisplayString().substr(indefinitePrefixLength);
            }
            else {
                displayString = npc(displayValue)->getDisplayString();
            }
            DisplayValue baseDisplayValue(displayString, npc(displayValue)->getConstraintMap());
            // We're asking for a specific form of definiteness, and it's not in that form already.
            if (definitePrefixLength <= 0 && definiteness->second == inflection::grammar::synthesis::GrammemeConstants::DEFINITENESS_DEFINITE()) {
                ::std::unique_ptr<SpeakableString> newDisplayString(npc(definiteFeatureFunction)->getFeatureValue(baseDisplayValue, constraints));
                if (newDisplayString) {
                    return replaceDisplayValue(displayValue, *newDisplayString);
                }
            }
            else if (indefinitePrefixLength <= 0 && definiteness->second == inflection::grammar::synthesis::GrammemeConstants::DEFINITENESS_INDEFINITE()) {
                ::std::unique_ptr<SpeakableString> newDisplayString(npc(indefiniteFeatureFunction)->getFeatureValue(baseDisplayValue, constraints));
                if (newDisplayString) {
                    return replaceDisplayValue(displayValue, *newDisplayString);
                }
            }
        }
    }
    return displayValue;
}

DisplayValue*
DefinitenessDisplayFunction::updateDefiniteness(DisplayValue* displayValue, const ::std::map<SemanticFeature, ::std::u16string>& constraints) const
{
    if (displayValue != nullptr) {
        auto definiteness = constraints.find(*npc(definiteFeature));
        if (definiteness != constraints.end()) {
            return addDefiniteness(displayValue, constraints);
        }
        std::u16string displayString;
        int32_t prefixLength = 0;
        if ((prefixLength = getArticlePrefixLength(displayValue, definiteArticles)) > 0) {
            displayString = npc(displayValue)->getDisplayString().substr(prefixLength);
            DisplayValue baseDisplayValue(displayString, npc(displayValue)->getConstraintMap());
            ::std::unique_ptr<SpeakableString> newDisplayString(npc(definiteFeatureFunction)->getFeatureValue(baseDisplayValue, constraints));
            if (newDisplayString) {
                return replaceDisplayValue(displayValue, *newDisplayString);
            }
        }
        else if ((prefixLength = getArticlePrefixLength(displayValue, indefiniteArticles)) > 0) {
            displayString = npc(displayValue)->getDisplayString().substr(prefixLength);
            DisplayValue baseDisplayValue(displayString, npc(displayValue)->getConstraintMap());
            ::std::unique_ptr<SpeakableString> newDisplayString(npc(indefiniteFeatureFunction)->getFeatureValue(baseDisplayValue, constraints));
            if (newDisplayString) {
                return replaceDisplayValue(displayValue, *newDisplayString);
            }
        }
    }
    return displayValue;
}

} // namespace inflection::dialog
