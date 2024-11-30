/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer.hpp>

#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_SvDisplayFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

void SvGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(DEF_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"den", u"det", u"de"));
    featureModel.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"en", u"ett", u"flera"));
    featureModel.putDefaultFeatureFunctionByName(INDEFINITE_PRONOUN, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"någon", u"något", u"några"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INDEFINITE_PRONOUN, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"någon", u"något", u"några"));
    featureModel.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"vilken", u"vilket", u"vilka"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"vilken", u"vilket", u"vilka"));
    featureModel.putDefaultFeatureFunctionByName(NEG_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ingen", u"inget", u"inga"));
    featureModel.putDefaultFeatureFunctionByName(WITH_NEG_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ingen", u"inget", u"inga"));
    featureModel.putDefaultFeatureFunctionByName(NEW_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ny", u"nytt", u"nya"));
    featureModel.putDefaultFeatureFunctionByName(WITH_NEW_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ny", u"nytt", u"nya"));
    featureModel.putDefaultFeatureFunctionByName(OTHER_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"annan", u"annat", u"andra"));
    featureModel.putDefaultFeatureFunctionByName(WITH_OTHER_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"annan", u"annat", u"andra"));
    featureModel.putDefaultFeatureFunctionByName(POSS_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"din", u"ditt", u"dina"));
    featureModel.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"din", u"ditt", u"dina"));
    featureModel.setDefaultDisplayFunction(new SvGrammarSynthesizer_SvDisplayFunction(featureModel));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::SWEDISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new SvGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::SWEDISH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()}));
}

SvGrammarSynthesizer::Count SvGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, SvGrammarSynthesizer::Count>({
        {GrammemeConstants::NUMBER_SINGULAR(), Count::singular},
        {GrammemeConstants::NUMBER_PLURAL(), Count::plural}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Count::undefined;
}

SvGrammarSynthesizer::Gender SvGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, SvGrammarSynthesizer::Gender>({
        {GrammemeConstants::GENDER_COMMON(), Gender::common},
        {GrammemeConstants::GENDER_NEUTER(), Gender::neuter}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Gender::undefined;
}

} // namespace morphuntion::grammar::synthesis
