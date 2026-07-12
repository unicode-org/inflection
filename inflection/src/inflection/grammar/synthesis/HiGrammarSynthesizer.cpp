/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HiGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_HiDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void HiGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::HINDI(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}, {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_VERB}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::grammar::GrammemeLookupFunction(
        ::inflection::util::LocaleUtils::HINDI(),
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE},
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADPOSITION, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB},
        GrammemeConstants::GENDER_MASCULINE, true,
        [](const ::std::u16string& word) -> ::std::u16string {
            static constexpr const char16_t* MASCULINE_SUFFIXES[] = {
                u"\u093E",  // ा
                u"\u0947",  // े (honorific)
            };
            static constexpr const char16_t* FEMININE_SUFFIXES[] = {
                u"\u0940",  // ी
                u"\u0947",  // े (honorific)
            };
            for (const auto& suffix : MASCULINE_SUFFIXES) {
                if (word.ends_with(suffix)) {
                    return GrammemeConstants::GENDER_MASCULINE;
                }
            }
            for (const auto& suffix : FEMININE_SUFFIXES) {
                if (word.ends_with(suffix)) {
                    return GrammemeConstants::GENDER_FEMININE;
                }
            }
            return {};
        }));
    featureModel.setDefaultDisplayFunction(new HiGrammarSynthesizer_HiDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis
