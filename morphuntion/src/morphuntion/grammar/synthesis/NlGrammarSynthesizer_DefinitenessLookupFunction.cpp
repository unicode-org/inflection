/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_DefinitenessLookupFunction::NlGrammarSynthesizer_DefinitenessLookupFunction()
    : super(::morphuntion::util::LocaleUtils::DUTCH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()})
{
}

morphuntion::dialog::SpeakableString* NlGrammarSynthesizer_DefinitenessLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        ::std::u16string lowercaseStr;
        ::morphuntion::util::StringViewUtils::lowercase(&lowercaseStr, displayString, morphuntion::util::LocaleUtils::ENGLISH());
        if (::morphuntion::util::StringViewUtils::startsWith(lowercaseStr, u"de ") || ::morphuntion::util::StringViewUtils::startsWith(lowercaseStr, u"het ")) {
            return new morphuntion::dialog::SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
        }
        if (::morphuntion::util::StringViewUtils::startsWith(lowercaseStr, u"een ")) {
            return new morphuntion::dialog::SpeakableString(GrammemeConstants::DEFINITENESS_INDEFINITE());
        }
    }
    auto result = determine(displayString);
    if (result.empty()) {
        return nullptr;
    }
    return new morphuntion::dialog::SpeakableString(result);
}

} // namespace morphuntion::grammar::synthesis
