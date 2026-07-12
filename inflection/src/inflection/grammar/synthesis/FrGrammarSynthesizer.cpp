/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer.hpp>

#include <inflection/dialog/ArticleDetectionFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_ArticleElidedFunction.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_FrDisplayFunction.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void FrGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto genderLookup = new ::inflection::grammar::GrammemeLookupFunction(
        ::inflection::util::LocaleUtils::FRENCH(),
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE},
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_DETERMINER, GrammemeConstants::POS_VERB},
        GrammemeConstants::GENDER_MASCULINE, true,
        [](const ::std::u16string& word) -> ::std::u16string {
            static const char16_t* const MASCULINE_SUFFIXES[] = {
                u"an", u"ans", u"en", u"ens", u"ge", u"ges",
                u"in", u"ins", u"ire", u"ires", u"lon", u"lons",
                u"me", u"mes", u"ste", u"stes", u"ton", u"tre", u"tres",
            };
            static const char16_t* const FEMININE_SUFFIXES[] = {
                u"a", u"e", u"es", u"n", u"ns", u"é", u"és",
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
        });
    const auto& genderLookupRef = *npc(genderLookup);
    auto numberLookup = new FrGrammarSynthesizer_CountLookupFunction();
    const auto& numberLookupRef = *npc(numberLookup);

    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, true, nullptr, u"", u"", u"le ", u"la ", u"l’", u"les "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, true, nullptr, u"", u"", u"un", u"une", u"", u"des"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_A_PREPOSITION, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, true, nullptr, u"à ", u"", u"au ", u"à la ", u"à l’", u"aux "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_A_PREPOSITION, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, true, true, ARTICLE_A_PREPOSITION, u"à ", u"", u"au ", u"à la ", u"à l’", u"aux "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DE_PREPOSITION, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, true, nullptr, u"de ", u"d’", u"du ", u"de la ", u"de l’", u"des "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DE_PREPOSITION, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, true, true, ARTICLE_DE_PREPOSITION, u"de ", u"d’", u"du ", u"de la ", u"de l’", u"des "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_CE_DEMONSTRATIVE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, false, nullptr, u"ce", u"", u"ce", u"cette", u"cet", u"ces"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_CE_DEMONSTRATIVE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  true, false, false, ARTICLE_CE_DEMONSTRATIVE, u"ce", u"", u"ce", u"cette", u"cet", u"ces"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DE_GENITIVE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, false, true, nullptr, u"de ", u"", u"de ", u"de la ", u"de l’", u"des "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DE_GENITIVE, new FrGrammarSynthesizer_ArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef,  false, true, true, ARTICLE_DE_GENITIVE, u"de ", u"", u"de ", u"de la ", u"de l’", u"des "));
    featureModel.putDefaultFeatureFunctionByName(ELISION_DE_PREPOSITION, new FrGrammarSynthesizer_ArticleElidedFunction(featureModel, false, nullptr, u"", u"de ", u"d’"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ELISION_DE_PREPOSITION, new FrGrammarSynthesizer_ArticleElidedFunction(featureModel, false, ELISION_DE_PREPOSITION, u"", u"de ", u"d’"));
    featureModel.putDefaultFeatureFunctionByName(ELISION_QUE, new FrGrammarSynthesizer_ArticleElidedFunction(featureModel, false, nullptr, u"", u"que ", u"qu’"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ELISION_QUE, new FrGrammarSynthesizer_ArticleElidedFunction(featureModel, false, ELISION_QUE, u"", u"que ", u"qu’"));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, genderLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, numberLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new inflection::dialog::ArticleDetectionFunction(inflection::util::LocaleUtils::FRENCH(),
             {ARTICLE_DEFINITE, ARTICLE_DE_PREPOSITION, ARTICLE_A_PREPOSITION, ARTICLE_DE_GENITIVE}, {u"d’", u"de "}, {ARTICLE_INDEFINITE}, {u"à "}));

    featureModel.setDefaultDisplayFunction(new FrGrammarSynthesizer_FrDisplayFunction(featureModel));
}

FrGrammarSynthesizer::Number FrGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

FrGrammarSynthesizer::Gender FrGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE) {
            return Gender::feminine;
        }
    }
    return Gender::undefined;
}

} // namespace inflection::grammar::synthesis
