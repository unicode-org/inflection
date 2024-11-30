/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/StaticArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog {

StaticArticleLookupFunction::StaticArticleLookupFunction(const SemanticFeatureModel& model, const char16_t* derivedSemanticName, const char16_t* articlePrefix)
    : super(model, true, true)
    , articlePrefix(articlePrefix)
    , articleSpacedPrefix(::std::u16string(articlePrefix) + u" ")
{
    this->derivedArticleFeature = (derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : static_cast<const SemanticFeature* >(nullptr));
}

SpeakableString* StaticArticleLookupFunction::getFeatureValue(const SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& /*constraints*/) const
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
        ::morphuntion::util::StringViewUtils::lowercase(&lowercaseStr, displayString, morphuntion::util::LocaleUtils::ENGLISH());
        if (!::morphuntion::util::StringViewUtils::startsWith(lowercaseStr, articleSpacedPrefix)) {
            return createPreposition(displayValue, articlePrefix);
        }
    }
    return createPreposition(displayValue, u"");
}

} // namespace morphuntion::dialog
