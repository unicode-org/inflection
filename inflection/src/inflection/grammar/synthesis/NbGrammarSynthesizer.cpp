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
    featureModel.putDefaultFeatureFunctionByName(DEF_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"den", u"den", u"det", u"de"));
    featureModel.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"en", u"ei", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"hvilken", u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"hvilken", u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(POSS_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"din", u"di", u"ditt", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"din", u"di", u"ditt", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(DEMON_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"denne", u"denne", u"dette", u"disse"));
    featureModel.putDefaultFeatureFunctionByName(WITH_DEMON_ARTICLE, new NbGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"denne", u"denne", u"dette", u"disse"));
    featureModel.setDefaultDisplayFunction(new NbGrammarSynthesizer_NbDisplayFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()}));
}

NbGrammarSynthesizer::Count NbGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, NbGrammarSynthesizer::Count>({
        {GrammemeConstants::NUMBER_SINGULAR(), Count::SINGULAR},
        {GrammemeConstants::NUMBER_PLURAL(), Count::PLURAL}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Count::UNDEFINED;
}

NbGrammarSynthesizer::Gender NbGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, NbGrammarSynthesizer::Gender>({
        {GrammemeConstants::GENDER_MASCULINE(), Gender::MASCULINE},
        {GrammemeConstants::GENDER_FEMININE(), Gender::FEMININE},
        {GrammemeConstants::GENDER_NEUTER(), Gender::NEUTER}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Gender::UNDEFINED;
}

} // namespace inflection::grammar::synthesis