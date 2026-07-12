/*
 * Copyright 2017-2025 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_DeDisplayFunction.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

typedef struct {
    DeGrammarSynthesizer::LookupKey key;
    const char16_t* adposition;
} KeyToAdposition;

template<auto& T>
static const char16_t * getArticle(DeGrammarSynthesizer::LookupKey key) {
    auto entry = inflection::util::ArrayUtils::searchSorted<T>(key, [](const auto& item) { return item.key; });
    if (entry != nullptr) {
        return entry->adposition;
    }
    return nullptr;
}

void DeGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& model)
{
    static constexpr KeyToAdposition POSSESSIVE_3RD_PRONOUNS[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"sie"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"ihnen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"ihrer"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"sie"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"sie"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"ihnen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"ihrer"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"sie"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"sie"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"ihnen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"ihrer"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"sie"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"sie"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"ihr"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"ihrer"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"sie"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"ihn"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"ihm"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"seiner"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"er"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"es"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"ihm"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"seiner"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"es"},
    };
    // TODO PRONOUN and WITH_PRONOUN are deprecated because it conflicts with pos('pronoun'). Remove them in the future. Though it would be better to use PronounConcept instead of this feature anyway.
    model.putDefaultFeatureFunctionByName(PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<POSSESSIVE_3RD_PRONOUNS>));
    model.putDefaultFeatureFunctionByName(WITH_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<POSSESSIVE_3RD_PRONOUNS>));
    model.putDefaultFeatureFunctionByName(POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<POSSESSIVE_3RD_PRONOUNS>));
    model.putDefaultFeatureFunctionByName(WITH_POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<POSSESSIVE_3RD_PRONOUNS>));

    static constexpr KeyToAdposition DEFINITE_ARTICLES[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"die"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"den"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"der"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"die"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"die"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"den"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"der"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"die"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"die"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"den"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"der"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"die"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"die"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"der"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"der"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"die"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"den"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"dem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"des"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"der"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"das"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"dem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"des"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"das"},
    };
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<DEFINITE_ARTICLES>));

    static constexpr KeyToAdposition DEFINITE_ARTICLES_IN_PREPOSITION[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"in die"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"in den"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"in der"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"in die"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"in die"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"in den"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"in der"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"in die"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"in die"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"in den"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"in der"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"in die"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"in die"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"in der"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"in der"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"in die"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"in den"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"im"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"in des"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"in der"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"ins"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"im"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"in des"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"ins"},
    };
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<DEFINITE_ARTICLES_IN_PREPOSITION>));
    model.putDefaultFeatureFunctionByName(WITH_DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<DEFINITE_ARTICLES_IN_PREPOSITION>));

    static constexpr KeyToAdposition INDEFINITE_ARTICLES[] = {
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"eine"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"einer"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"einer"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"eine"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"einen"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"einem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"eines"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"ein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"ein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"einem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"eines"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"ein"},
    };
    model.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<INDEFINITE_ARTICLES>));

    static constexpr KeyToAdposition DEMONSTRATIVE_ARTICLES[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"dieser"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"diese"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"dieser"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"diese"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"diesen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"dieser"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"diese"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"diese"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"dieser"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"dieser"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"diese"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"diesen"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"diesem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"dieses"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"dieser"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"dieses"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"diesem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"dieses"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"dieses"},
    };
    model.putDefaultFeatureFunctionByName(DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<DEMONSTRATIVE_ARTICLES>));
    model.putDefaultFeatureFunctionByName(WITH_DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<DEMONSTRATIVE_ARTICLES>));

    static constexpr KeyToAdposition NEGATED_ARTICLES[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"keine"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"keinen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"keiner"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"keine"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"keine"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"keinen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"keiner"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"keine"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"keine"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"keinen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"keiner"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"keine"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"keine"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"keiner"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"keiner"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"keine"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"keinen"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"keinem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"keines"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"kein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"kein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"keinem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"keines"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"kein"},
    };
    model.putDefaultFeatureFunctionByName(NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<NEGATED_ARTICLES>));
    model.putDefaultFeatureFunctionByName(WITH_NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<NEGATED_ARTICLES>));

    static constexpr KeyToAdposition POSSESSIVE_2ND_PRONOUNS[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"deine"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"deinen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"deiner"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"deine"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"deine"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"deinen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"deiner"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"deine"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"deine"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"deinen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"deiner"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"deine"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"deine"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"deiner"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"deiner"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"deine"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"deinen"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"deinem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"deines"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"dein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"dein"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"deinem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"deines"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"dein"},
    };
    model.putDefaultFeatureFunctionByName(POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<POSSESSIVE_2ND_PRONOUNS>));
    model.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<POSSESSIVE_2ND_PRONOUNS>));

    static constexpr KeyToAdposition INTERROGATIVE_ARTICLES[] = {
        {makeLookupKey(Number::plural, Gender::feminine, Case::accusative), u"welche"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::dative), u"welchen"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::genitive), u"welcher"},
        {makeLookupKey(Number::plural, Gender::feminine, Case::nominative), u"welche"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::accusative), u"welche"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::dative), u"welchen"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::genitive), u"welcher"},
        {makeLookupKey(Number::plural, Gender::masculine, Case::nominative), u"welche"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::accusative), u"welche"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::dative), u"welchen"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::genitive), u"welcher"},
        {makeLookupKey(Number::plural, Gender::neuter, Case::nominative), u"welche"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::accusative), u"welche"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::dative), u"welcher"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::genitive), u"welcher"},
        {makeLookupKey(Number::singular, Gender::feminine, Case::nominative), u"welche"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::accusative), u"welchen"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::dative), u"welchem"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::genitive), u"welches"},
        {makeLookupKey(Number::singular, Gender::masculine, Case::nominative), u"welcher"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::accusative), u"welches"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::dative), u"welchem"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::genitive), u"welches"},
        {makeLookupKey(Number::singular, Gender::neuter, Case::nominative), u"welches"},
    };
    model.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, getArticle<INTERROGATIVE_ARTICLES>));
    model.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, getArticle<INTERROGATIVE_ARTICLES>));

    model.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::GERMAN(), {GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_NEUTER}));
    model.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::GERMAN(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}));

    model.setDefaultDisplayFunction(new DeGrammarSynthesizer_DeDisplayFunction(model));
}

DeGrammarSynthesizer::Number DeGrammarSynthesizer::getNumber(const ::std::u16string* value) {
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

DeGrammarSynthesizer::Gender DeGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE) {
            return Gender::feminine;
        }
        if (valRef == GrammemeConstants::GENDER_NEUTER) {
            return Gender::neuter;
        }
    }
    return Gender::undefined;
}

DeGrammarSynthesizer::Case DeGrammarSynthesizer::getCase(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::CASE_NOMINATIVE) {
            return Case::nominative;
        }
        if (valRef == GrammemeConstants::CASE_ACCUSATIVE) {
            return Case::accusative;
        }
        if (valRef == GrammemeConstants::CASE_DATIVE) {
            return Case::dative;
        }
        if (valRef == GrammemeConstants::CASE_GENITIVE) {
            return Case::genitive;
        }
    }
    return Case::undefined;
}

} // namespace inflection::grammar::synthesis
