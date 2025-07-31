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

TrGrammarSynthesizer::Number TrGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR()) {
            return Number::SINGULAR;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL()) {
            return Number::PLURAL;
        }
    }
    return Number::undefined;
}

TrGrammarSynthesizer::Person TrGrammarSynthesizer::getPerson(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::PERSON_FIRST()) {
            return Person::FIRST;
        }
        if (valRef == GrammemeConstants::PERSON_SECOND()) {
            return Person::SECOND;
        }
        if (valRef == GrammemeConstants::PERSON_THIRD()) {
            return Person::THIRD;
        }
    }
    return Person::undefined;
}

TrGrammarSynthesizer::Pronoun TrGrammarSynthesizer::getPronoun(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR()) {
            return Pronoun::SINGULAR;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL()) {
            return Pronoun::PLURAL;
        }
    }
    return Pronoun::undefined;
}

TrGrammarSynthesizer::Copula TrGrammarSynthesizer::getCopula(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef ==  GrammemeConstants::PERSON_FIRST()) {
            return Copula::FIRST;
        }
        if (valRef == GrammemeConstants::PERSON_SECOND()) {
            return Copula::SECOND;
        }
        if (valRef == GrammemeConstants::PERSON_THIRD()) {
            return Copula::THIRD;
        }
    }
    return Copula::undefined;
}


TrGrammarSynthesizer::Tense TrGrammarSynthesizer::getTense(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::TENSE_PRESENT()) {
            return Tense::present;
        }
        if (valRef == GrammemeConstants::TENSE_PAST()) {
            return Tense::past;
        }
    }
    return Tense::undefined;
}

} // namespace inflection::grammar::synthesis
