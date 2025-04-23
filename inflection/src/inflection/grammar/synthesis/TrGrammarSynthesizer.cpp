/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>

#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_DeConjunctionArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_InterrogativeArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_TrDisplayFunction.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

const ::std::u16string& TrGrammarSynthesizer::PRONOUN()
{
    static auto PRONOUN_ = new ::std::u16string(u"pronoun");
    return *npc(PRONOUN_);
}

const ::std::u16string& TrGrammarSynthesizer::PRONOUN_NUMBER()
{
    static auto PRONOUN_ = new ::std::u16string(u"pronounNumber");
    return *npc(PRONOUN_);
}

const ::std::u16string& TrGrammarSynthesizer::COPULA()
{
    static auto COPULA_ = new ::std::u16string(u"copula");
    return *npc(COPULA_);
}

const ::std::u16string& TrGrammarSynthesizer::TENSE()
{
    static auto TENSE_ = new ::std::u16string(u"tense");
    return *npc(TENSE_);
}

const ::std::u16string& TrGrammarSynthesizer::COPULA_FIRST()
{
    static auto COPULA_FIRST_ = new ::std::u16string(u"first");
    return *npc(COPULA_FIRST_);
}

const ::std::u16string& TrGrammarSynthesizer::COPULA_SECOND()
{
    static auto COPULA_SECOND_ = new ::std::u16string(u"second");
    return *npc(COPULA_SECOND_);
}

const ::std::u16string& TrGrammarSynthesizer::COPULA_THIRD()
{
    static auto COPULA_THIRD_ = new ::std::u16string(u"third");
    return *npc(COPULA_THIRD_);
}

const ::std::u16string& TrGrammarSynthesizer::TENSE_PRESENT()
{
    static auto TENSE_PRESENT_ = new ::std::u16string(u"present");
    return *npc(TENSE_PRESENT_);
}

const ::std::u16string& TrGrammarSynthesizer::TENSE_PAST()
{
    static auto TENSE_PAST_ = new ::std::u16string(u"past");
    return *npc(TENSE_PAST_);
}

const ::icu4cxx::UnicodeSet& TrGrammarSynthesizer::SEPARATOR_SPLITTER()
{
    static auto SEPARATOR_SPLITTER_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[/:[:whitespace:]]"));
    return *npc(SEPARATOR_SPLITTER_);
}

void TrGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto displayFunction = new TrGrammarSynthesizer_TrDisplayFunction(featureModel);
    featureModel.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new TrGrammarSynthesizer_InterrogativeArticleLookupFunction(featureModel, displayFunction));
    featureModel.putDefaultFeatureFunctionByName(DE_CONJUNCTION, new TrGrammarSynthesizer_DeConjunctionArticleLookupFunction(featureModel, displayFunction));
    featureModel.setDefaultDisplayFunction(displayFunction);

}

char16_t TrGrammarSynthesizer::getAffix(TrGrammarSynthesizer::VowelGroup value) {
    static const char16_t AFFIX[] = {u'i', u'ü', u'ı', u'u'};
    return AFFIX[(int32_t)value];
}

TrGrammarSynthesizer::Case TrGrammarSynthesizer::getCase(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Case>({
        {GrammemeConstants::CASE_ABLATIVE(), Case::ABLATIVE},
        {GrammemeConstants::CASE_ACCUSATIVE(), Case::ACCUSATIVE},
        {GrammemeConstants::CASE_DATIVE(), Case::DATIVE},
        {GrammemeConstants::CASE_GENITIVE(), Case::GENITIVE},
        {GrammemeConstants::CASE_LOCATIVE(), Case::LOCATIVE},
        {GrammemeConstants::CASE_INSTRUMENTAL(), Case::INSTRUMENTAL}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Case::undefined;
}

TrGrammarSynthesizer::Count TrGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Count>({
        {GrammemeConstants::NUMBER_SINGULAR(), Count::SINGULAR},
        {GrammemeConstants::NUMBER_PLURAL(), Count::PLURAL}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Count::undefined;
}

TrGrammarSynthesizer::Person TrGrammarSynthesizer::getPerson(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Person>({
        {GrammemeConstants::PERSON_FIRST(), Person::FIRST},
        {GrammemeConstants::PERSON_SECOND(), Person::SECOND},
        {GrammemeConstants::PERSON_THIRD(), Person::THIRD}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Person::undefined;
}

TrGrammarSynthesizer::Pronoun TrGrammarSynthesizer::getPronoun(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Pronoun>({
        {GrammemeConstants::NUMBER_SINGULAR(), Pronoun::SINGULAR},
        {GrammemeConstants::NUMBER_PLURAL(), Pronoun::PLURAL}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Pronoun::undefined;
}

TrGrammarSynthesizer::Copula TrGrammarSynthesizer::getCopula(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Copula>({
        {COPULA_FIRST(), Copula::FIRST},
        {COPULA_SECOND(), Copula::SECOND},
        {COPULA_THIRD(), Copula::THIRD}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Copula::undefined;
}


TrGrammarSynthesizer::Tense TrGrammarSynthesizer::getTense(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, TrGrammarSynthesizer::Tense>({
        {TENSE_PRESENT(), Tense::present},
        {TENSE_PAST(), Tense::past}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Tense::undefined;
}

} // namespace inflection::grammar::synthesis
