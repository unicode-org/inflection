/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer.hpp>

#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer_DeDisplayFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

const ::std::u16string& DeGrammarSynthesizer::DECLENSION()
{
    static auto DECLENSION_ = new ::std::u16string(u"declension");
    return *npc(DECLENSION_);
}

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
void DeGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& model)
{
    ::std::map<int32_t, ::std::u16string_view> pronouns;
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"er");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"seiner");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"ihm");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"ihn");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"ihr");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"es");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"seiner");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"ihm");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"es");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"sie");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"ihrer");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"ihnen");
    pronouns.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"sie");
    // TODO PRONOUN and WITH_PRONOUN are deprecated because it conflicts with pos('pronoun'). Remove them in the future. Though it would be better to use PronounConcept instead of this feature anyway.
    model.putDefaultFeatureFunctionByName(PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, pronouns));
    model.putDefaultFeatureFunctionByName(WITH_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, pronouns));
    model.putDefaultFeatureFunctionByName(POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, pronouns));
    model.putDefaultFeatureFunctionByName(WITH_POSSESSIVE_PRONOUN, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, pronouns));
    ::std::map<int32_t, ::std::u16string_view> defArticles;
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"des");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"dem");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"das");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"des");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"dem");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"das");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"die");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"der");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"den");
    defArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"die");
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, defArticles));
    ::std::map<int32_t, ::std::u16string_view> defArticlesInPreposition;
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"in des");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"im");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"ins");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"in des");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"im");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"ins");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"in die");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"in der");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"in den");
    defArticlesInPreposition.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"in die");
    model.putDefaultFeatureFunctionByName(DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, defArticlesInPreposition));
    model.putDefaultFeatureFunctionByName(WITH_DEF_ARTICLE_IN_PREPOSITION, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, defArticlesInPreposition));

    ::std::map<int32_t, ::std::u16string_view> indefArticles;
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"ein");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"eines");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"einem");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"einen");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"eine");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"einer");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"einer");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"eine");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"ein");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"eines");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"einem");
    indefArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"ein");
    model.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, indefArticles));
    ::std::map<int32_t, ::std::u16string_view> demonstrativeArticles;
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"diesem");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"diesem");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"dieses");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"diese");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"dieser");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"diesen");
    demonstrativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"diesen");
    model.putDefaultFeatureFunctionByName(DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, demonstrativeArticles));
    model.putDefaultFeatureFunctionByName(WITH_DEMON_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, demonstrativeArticles));
    ::std::map<int32_t, ::std::u16string_view> negatedArticles;
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"keines");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"keinem");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"keines");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"keinem");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"kein");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"keine");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"keiner");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"keinen");
    negatedArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"keine");
    model.putDefaultFeatureFunctionByName(NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, negatedArticles));
    model.putDefaultFeatureFunctionByName(WITH_NEG_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, negatedArticles));
    ::std::map<int32_t, ::std::u16string_view> possessiveArticles;
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"deines");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"deinem");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"deines");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"deinem");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"dein");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"deine");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"deiner");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"deinen");
    possessiveArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"deine");
    model.putDefaultFeatureFunctionByName(POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, possessiveArticles));
    model.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, possessiveArticles));
    ::std::map<int32_t, ::std::u16string_view> interrogativeArticles;
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"welchem");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"welchem");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"welches");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"welche");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"welcher");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"welchen");
    interrogativeArticles.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"welche");
    model.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, false, interrogativeArticles));
    model.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new DeGrammarSynthesizer_ArticleLookupFunction(model, true, interrogativeArticles));
    ::std::map<int32_t, ::std::u16string_view> strongSuffixes;
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"em");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"es");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"em");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"es");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"e");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"er");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"en");
    strongSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"e");
    ::std::map<int32_t, ::std::u16string_view> weakSuffixes;
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"e");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"en");
    weakSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"en");
    ::std::map<int32_t, ::std::u16string_view> mixedSuffixes;
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"er");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"e");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"e");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"es");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::singular, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"es");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::nominative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::masculine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::nominative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::feminine, DeGrammarSynthesizer::Case::accusative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::nominative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::genitive).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::dative).value, u"en");
    mixedSuffixes.emplace(makeLookupKey(DeGrammarSynthesizer::Count::plural, DeGrammarSynthesizer::Gender::neuter, DeGrammarSynthesizer::Case::accusative).value, u"en");
    model.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::GERMAN(), {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_NEUTER()}));
    model.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::GERMAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    model.setDefaultDisplayFunction(new DeGrammarSynthesizer_DeDisplayFunction(model, strongSuffixes, weakSuffixes, mixedSuffixes));
}

DeGrammarSynthesizer::Count DeGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, DeGrammarSynthesizer::Count>({
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

DeGrammarSynthesizer::Gender DeGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, DeGrammarSynthesizer::Gender>({
        {GrammemeConstants::GENDER_MASCULINE(), Gender::masculine},
        {GrammemeConstants::GENDER_FEMININE(), Gender::feminine},
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

DeGrammarSynthesizer::Case DeGrammarSynthesizer::getCase(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, DeGrammarSynthesizer::Case>({
        {GrammemeConstants::CASE_NOMINATIVE(), Case::nominative},
        {GrammemeConstants::CASE_ACCUSATIVE(), Case::accusative},
        {GrammemeConstants::CASE_DATIVE(), Case::dative},
        {GrammemeConstants::CASE_GENITIVE(), Case::genitive}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Case::undefined;
}

DeGrammarSynthesizer::LookupKey DeGrammarSynthesizer::makeLookupKey(Count field0, Gender field1, Case field2) {
    LookupKey retVal {.value = 0};
    retVal.fields.field0 = (uint8_t)field0;
    retVal.fields.field1 = (uint8_t)field1;
    retVal.fields.field2 = (uint8_t)field2;
    return retVal;
}

} // namespace morphuntion::grammar::synthesis
