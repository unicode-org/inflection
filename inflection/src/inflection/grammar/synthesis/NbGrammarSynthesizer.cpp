/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NbGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer_NbDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

void NbGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto numberLookup = new inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL});
    const auto& numberLookupRef = *npc(numberLookup);
    auto genderLookup = new inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER});
    const auto& genderLookupRef = *npc(genderLookup);
    featureModel.putDefaultFeatureFunctionByName(DEF_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, u"den", u"den", u"det", u"de"));
    featureModel.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, u"en", u"ei", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, u"hvilken", u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  true, u"hvilken", u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(POSS_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, u"din", u"di", u"ditt", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  true, u"din", u"di", u"ditt", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(DEMON_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, u"denne", u"denne", u"dette", u"disse"));
    featureModel.putDefaultFeatureFunctionByName(WITH_DEMON_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  true, u"denne", u"denne", u"dette", u"disse"));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, numberLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, genderLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::DEFINITENESS_DEFINITE, GrammemeConstants::DEFINITENESS_INDEFINITE}));
    featureModel.setDefaultDisplayFunction(new NbGrammarSynthesizer_NbDisplayFunction(featureModel, genderLookupRef));
}

NbGrammarSynthesizer::Number NbGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR) {
            return Number::SINGULAR;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL) {
            return Number::PLURAL;
        }
    }
    return Number::UNDEFINED;
}

NbGrammarSynthesizer::Gender NbGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE) {
            return Gender::MASCULINE;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE) {
            return Gender::FEMININE;
        }
        if (valRef == GrammemeConstants::GENDER_NEUTER) {
            return Gender::NEUTER;
        }
    }
    return Gender::UNDEFINED;
}

} // namespace inflection::grammar::synthesis
