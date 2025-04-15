/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/StaticArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

StaticArticleLookupFunction::StaticArticleLookupFunction(const SemanticFeatureModel& model, const char16_t* derivedSemanticName, const char16_t* articlePrefix)
    : super(model, true, true)
    , articlePrefix(articlePrefix)
    , articleSpacedPrefix(::std::u16string(articlePrefix) + u" ")
{
    this->derivedArticleFeature = (derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : static_cast<const SemanticFeature* >(nullptr));
}

SpeakableString* StaticArticleLookupFunction::getFeatureValue(const DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto& displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        ::std::u16string lowercaseStr;
        ::inflection::util::StringViewUtils::lowercase(&lowercaseStr, displayString, inflection::util::LocaleUtils::ENGLISH());
        if (!lowercaseStr.starts_with(articleSpacedPrefix)) {
            return createPreposition(displayValue, articlePrefix);
        }
    }
    return createPreposition(displayValue, u"");
}

} // namespace inflection::dialog
