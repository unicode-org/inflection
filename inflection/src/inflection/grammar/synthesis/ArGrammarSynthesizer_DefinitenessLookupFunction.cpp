/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 1/31/24.
//
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <map>
#include <string>

namespace inflection::grammar::synthesis {

using util::StringViewUtils;
using util::LocaleUtils;
using util::Validate;
using dialog::SpeakableString;

#define DEFINITENESS_VALUES {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE(), GrammemeConstants::DEFINITENESS_CONSTRUCT()}

ArGrammarSynthesizer_DefinitenessLookupFunction::ArGrammarSynthesizer_DefinitenessLookupFunction(const ::std::map<int32_t, ::std::u16string_view> &pronounMap)
    : super(LocaleUtils::ARABIC(), DEFINITENESS_VALUES)
    , pronounMap(pronounMap)
{
    Validate::notNull(getDictionary().getBinaryProperties(&properNounMask, {GrammemeConstants::POS_PROPER_NOUN()}));
    Validate::notNull(getDictionary().getBinaryProperties(&definitePOSMask, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_PARTICLE()}));
    Validate::notNull(getDictionary().getBinaryProperties(&definitenessMask, DEFINITENESS_VALUES));
}

SpeakableString* ArGrammarSynthesizer_DefinitenessLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const {
    const auto& displayString = displayValue.getDisplayString();
    const auto &dictionary = getDictionary();
    // Return nullptr for empty string
    if (displayString.empty()) {
        return nullptr;
    }
    
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, displayString);
    // Proper-nouns are definite
    if ((wordGrammemes & properNounMask) != 0) {
        return new SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
    }
    // Perform dictionary lookup for definiteness state
    if (const auto result = determine(displayString); !result.empty()) {
        return new SpeakableString(result);
    }

    // Return nullptr for ambiguous cases
    if ((wordGrammemes & definitenessMask) != 0) {
        return nullptr;
    }

    if (!displayString.empty()) {
        ::std::u16string lowercaseStr;
        StringViewUtils::lowercase(&lowercaseStr, displayString, inflection::util::LocaleUtils::ARABIC());
        // Start with "al" then definite
        if (lowercaseStr.starts_with(u"ال")) {
            return new SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
        }

        // Ends with a possessive pronoun with prefix being a noun or adjective implies definite
        for (const auto &[key, possessivePronoun]: pronounMap) {
            if (lowercaseStr.ends_with(possessivePronoun)) {
                const auto &prefix = lowercaseStr.substr(0, lowercaseStr.size() - possessivePronoun.size());
                int64_t prefixGrammemes = 0;
                dictionary.getCombinedBinaryType(&prefixGrammemes, prefix);
                if ((prefixGrammemes & definitePOSMask) != 0) {
                    return new SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
                }
            }
        }
    }
    return nullptr;
}

} // namespace inflection::grammar::synthesis
