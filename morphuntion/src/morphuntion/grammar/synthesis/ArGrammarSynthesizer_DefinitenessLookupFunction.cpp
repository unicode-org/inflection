/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 1/31/24.
//
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_DefinitenessLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <map>
#include <string>

namespace morphuntion::grammar::synthesis {

using util::StringViewUtils;
using util::LocaleUtils;
using util::Validate;
using dialog::SpeakableString;

static const std::vector<std::u16string>& DEFINITENESS_VALUES() {
    static std::vector<std::u16string>* _DEFINITENESS_VALUES = new std::vector<std::u16string>({GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE(), GrammemeConstants::DEFINITENESS_CONSTRUCT()});
    return *_DEFINITENESS_VALUES;
}

ArGrammarSynthesizer_DefinitenessLookupFunction::ArGrammarSynthesizer_DefinitenessLookupFunction(const ::std::map<int32_t, ::std::u16string_view> &pronounMap)
    : super(LocaleUtils::ARABIC(), DEFINITENESS_VALUES())
    , pronounMap(pronounMap)
{
    Validate::notNull(getDictionary().getBinaryProperties(&properNounMask, {GrammemeConstants::POS_PROPER_NOUN()}));
    Validate::notNull(getDictionary().getBinaryProperties(&definitePOSMask, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_PARTICLE()}));
    Validate::notNull(getDictionary().getBinaryProperties(&definitenessMask, DEFINITENESS_VALUES()));
}

SpeakableString* ArGrammarSynthesizer_DefinitenessLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const {
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
        StringViewUtils::lowercase(&lowercaseStr, displayString, morphuntion::util::LocaleUtils::ARABIC());
        // Start with "al" then definite
        if (StringViewUtils::startsWith(lowercaseStr, u"ال")) {
            return new SpeakableString(GrammemeConstants::DEFINITENESS_DEFINITE());
        }

        // Ends with a possessive pronoun with prefix being a noun or adjective implies definite
        for (const auto &[key, possessivePronoun]: pronounMap) {
            if (StringViewUtils::endsWith(lowercaseStr, possessivePronoun)) {
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

} // namespace morphuntion::grammar::synthesis
