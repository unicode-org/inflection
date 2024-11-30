/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer.hpp>

#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer_DaDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

void DaGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_NEGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ingen", u"intet", u"ingen"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_NEGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ingen", u"intet", u"ingen"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, ::morphuntion::dialog::SpeakableString(u"den", u"dén"), ::morphuntion::dialog::SpeakableString(u"det"), ::morphuntion::dialog::SpeakableString(u"de")));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, ::morphuntion::dialog::SpeakableString(u"den", u"dén"), ::morphuntion::dialog::SpeakableString(u"det"), ::morphuntion::dialog::SpeakableString(u"de")));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"en", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"en", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_POSSESSIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"din", u"dit", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_POSSESSIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"din", u"dit", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_NEW, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ny", u"nyt", u"nye"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_NEW, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ny", u"nyt", u"nye"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_OTHER, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"anden", u"andet", u"andre"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_OTHER, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"anden", u"andet", u"andre"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INTERROGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_INTERROGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(PRONOUN_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"nogen", u"noget", u"nogle"));
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"nogen", u"noget", u"nogle"));
    featureModel.putDefaultFeatureFunctionByName(PRONOUN_DEMONSTRATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"denne", u"dette", u"disse"));
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_DEMONSTRATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"denne", u"dette", u"disse"));
    featureModel.setDefaultDisplayFunction(new DaGrammarSynthesizer_DaDisplayFunction(featureModel));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()}));
}

DaGrammarSynthesizer::Count DaGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, DaGrammarSynthesizer::Count>({
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

DaGrammarSynthesizer::Gender DaGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, DaGrammarSynthesizer::Gender>({
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
