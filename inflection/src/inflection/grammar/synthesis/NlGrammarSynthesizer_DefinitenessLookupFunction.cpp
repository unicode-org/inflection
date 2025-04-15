/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_DefinitenessLookupFunction::NlGrammarSynthesizer_DefinitenessLookupFunction()
    : super(::inflection::util::LocaleUtils::DUTCH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()})
{
}

inflection::dialog::SpeakableString* NlGrammarSynthesizer_DefinitenessLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        ::std::u16string lowercaseStr;
        ::inflection::util::StringViewUtils::lowercase(&lowercaseStr, displayString, inflection::util::LocaleUtils::ENGLISH());
        if (lowercaseStr.starts_with(u"de ") || lowercaseStr.starts_with(u"het ")) {
            return new inflection::dialog::SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
        }
        if (lowercaseStr.starts_with(u"een ")) {
            return new inflection::dialog::SpeakableString(GrammemeConstants::DEFINITENESS_INDEFINITE());
        }
    }
    auto result = determine(displayString);
    if (result.empty()) {
        return nullptr;
    }
    return new inflection::dialog::SpeakableString(result);
}

} // namespace inflection::grammar::synthesis
