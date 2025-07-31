/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_DeDisplayFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

const ::std::u16string& DeGrammarSynthesizer::DECLENSION_STRONG()
{
    static auto DECLENSION_STRONG_ = new ::std::u16string(u"strong");
    return *npc(DECLENSION_STRONG_);
}

const ::std::u16string& DeGrammarSynthesizer::DECLENSION_MIXED()
{
    static auto DECLENSION_MIXED_ = new ::std::u16string(u"mixed");
    return *npc(DECLENSION_MIXED_);
}

const ::std::u16string& DeGrammarSynthesizer::DECLENSION_WEAK()
{
    static auto DECLENSION_WEAK_ = new ::std::u16string(u"weak");
    return *npc(DECLENSION_WEAK_);
}

void DeGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& model)
{
    ::std::map<int32_t, ::std::u16string_view> pronouns;
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"er");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"seiner");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"ihm");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"ihn");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"ihr");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"es");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"seiner");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"ihm");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"es");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"sie");
    // TODO PRONOUN and WITH_PRONOUN are deprecated because it conflicts with pos('pronoun'). Remove them in the future. Though it would be better to use PronounConcept instead of this feature anyway.
    model.putDefaultFeatureFunctionByName(PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, pronouns));
    model.putDefaultFeatureFunctionByName(WITH_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, pronouns));
    model.putDefaultFeatureFunctionByName(POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, pronouns));
    model.putDefaultFeatureFunctionByName(WITH_POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, pronouns));
    ::std::map<int32_t, ::std::u16string_view> defArticles;
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"des");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"dem");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"das");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"des");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"dem");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"das");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"die");
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, defArticles));
    ::std::map<int32_t, ::std::u16string_view> defArticlesInPreposition;
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"in des");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"im");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"ins");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"in des");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"im");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"ins");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"in die");
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, defArticlesInPreposition));
    model.putDefaultFeatureFunctionByName(WITH_DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, defArticlesInPreposition));

    ::std::map<int32_t, ::std::u16string_view> indefArticles;
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"ein");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"eines");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"einem");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"einen");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"eine");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"einer");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"einer");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"eine");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"ein");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"eines");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"einem");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"ein");
    model.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, indefArticles));
    ::std::map<int32_t, ::std::u16string_view> demonstrativeArticles;
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"diesem");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"diesem");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"diesen");
    model.putDefaultFeatureFunctionByName(DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, demonstrativeArticles));
    model.putDefaultFeatureFunctionByName(WITH_DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, demonstrativeArticles));
    ::std::map<int32_t, ::std::u16string_view> negatedArticles;
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"keines");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"keinem");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"keines");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"keinem");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"keine");
    model.putDefaultFeatureFunctionByName(NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, negatedArticles));
    model.putDefaultFeatureFunctionByName(WITH_NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, negatedArticles));
    ::std::map<int32_t, ::std::u16string_view> possessiveArticles;
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"deines");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"deinem");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"deines");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"deinem");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"deine");
    model.putDefaultFeatureFunctionByName(POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, possessiveArticles));
    model.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, possessiveArticles));
    ::std::map<int32_t, ::std::u16string_view> interrogativeArticles;
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"welchem");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"welchem");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"welche");
    model.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, interrogativeArticles));
    model.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, interrogativeArticles));
    ::std::map<int32_t, ::std::u16string_view> strongSuffixes;
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"em");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"es");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"em");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"es");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"e");
    ::std::map<int32_t, ::std::u16string_view> weakSuffixes;
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"en");
    ::std::map<int32_t, ::std::u16string_view> mixedSuffixes;
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"er");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"e");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"e");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"es");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"es");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative), u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Number::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative), u"en");
    model.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::GERMAN(), {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_NEUTER()}));
    model.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::GERMAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    model.setDefaultDisplayFunction(new DeGrammarSynthesizer_DeDisplayFunction(model, strongSuffixes, weakSuffixes, mixedSuffixes));
}

DeGrammarSynthesizer::Number DeGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR()) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL()) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

DeGrammarSynthesizer::Gender DeGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE()) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE()) {
            return Gender::feminine;
        }
        if (valRef == GrammemeConstants::GENDER_NEUTER()) {
            return Gender::neuter;
        }
    }
    return Gender::undefined;
}

DeGrammarSynthesizer::Case DeGrammarSynthesizer::getCase(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::CASE_NOMINATIVE()) {
            return Case::nominative;
        }
        if (valRef == GrammemeConstants::CASE_ACCUSATIVE()) {
            return Case::accusative;
        }
        if (valRef == GrammemeConstants::CASE_DATIVE()) {
            return Case::dative;
        }
        if (valRef == GrammemeConstants::CASE_GENITIVE()) {
            return Case::genitive;
        }
    }
    return Case::undefined;
}

DeGrammarSynthesizer::LookupKey DeGrammarSynthesizer::makeLookupKey(Number field0, Gender field1, Case field2) {
    return (static_cast<LookupKey>(field2) << 8)
        | (static_cast<LookupKey>(field1) << 4)
        | static_cast<LookupKey>(field0);
}

} // namespace inflection::grammar::synthesis
