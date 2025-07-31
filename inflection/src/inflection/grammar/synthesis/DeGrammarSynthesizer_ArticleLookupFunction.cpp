/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

DeGrammarSynthesizer_ArticleLookupFunction::DeGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::map<int32_t, ::std::u16string_view>& articleMap)
    : super(model, includeSemanticValue, true)
    , dictionary(*::inflection::util::Validate::notNull(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::GERMAN())))
    , caseFeature(::inflection::util::Validate::notNull(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(::inflection::util::Validate::notNull(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(::inflection::util::Validate::notNull(model.getFeature(GrammemeConstants::GENDER)))
    , articleMap(articleMap)
    , tokenizer(::inflection::util::Validate::notNull(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::GERMAN())))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {u"neuter"}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {u"singular"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    dictionaryCount = dictionarySingular | dictionaryPlural;
}

DeGrammarSynthesizer_ArticleLookupFunction::DeGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const DeGrammarSynthesizer_ArticleLookupFunction& other)
    : DeGrammarSynthesizer_ArticleLookupFunction(model, includeSemanticValue, other.articleMap)
{
}

DeGrammarSynthesizer_ArticleLookupFunction::~DeGrammarSynthesizer_ArticleLookupFunction()
{

}

inflection::dialog::SpeakableString* DeGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    auto lookupKey = getArticleKey(&displayValue);
    auto preposition = articleMap.find(lookupKey);
    if (preposition == articleMap.end()) {
        return createPreposition(displayValue, ::std::u16string());
    }
    return createPreposition(displayValue, ::std::u16string(preposition->second));
}

DeGrammarSynthesizer::LookupKey DeGrammarSynthesizer_ArticleLookupFunction::getArticleKey(const ::inflection::dialog::DisplayValue* displayValue) const
{
    auto caseValue = DeGrammarSynthesizer::getCase(npc(displayValue)->getFeatureValue(*npc(caseFeature)));
    auto countValue = DeGrammarSynthesizer::getNumber(npc(displayValue)->getFeatureValue(*npc(countFeature)));
    auto genderValue = DeGrammarSynthesizer::getGender(npc(displayValue)->getFeatureValue(*npc(genderFeature)));
    int64_t phraseType = 0;
    if (countValue == DeGrammarSynthesizer::Number::undefined || genderValue == DeGrammarSynthesizer::Gender::undefined) {
        auto displayString = npc(displayValue)->getDisplayString();
        if (dictionary.getCombinedBinaryType(&phraseType, displayString) == nullptr) {
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> words(npc(tokenizer.get())->createTokenChain(displayString));
            if (words->getWordCount() > 1) {
                dictionary.getCombinedBinaryType(&phraseType, npc(npc(words->getEnd())->getPrevious())->getValue());
            }
        }
    }
    if (caseValue == DeGrammarSynthesizer::Case::undefined) {
        caseValue = DeGrammarSynthesizer::Case::nominative;
    }
    if (countValue == DeGrammarSynthesizer::Number::undefined) {
        if (phraseType != 0 && (phraseType & dictionaryCount) == dictionaryPlural) {
            countValue = DeGrammarSynthesizer::Number::plural;
        }
        else {
            countValue = DeGrammarSynthesizer::Number::singular;
        }
    }
    if (genderValue == DeGrammarSynthesizer::Gender::undefined && phraseType != 0) {
        if ((phraseType & dictionaryGenderMask) == dictionaryFeminine) {
            genderValue = DeGrammarSynthesizer::Gender::feminine;
        }
        else if ((phraseType & dictionaryGenderMask) == dictionaryMasculine) {
            genderValue = DeGrammarSynthesizer::Gender::masculine;
        }
        else if ((phraseType & dictionaryGenderMask) == dictionaryNeuter) {
            genderValue = DeGrammarSynthesizer::Gender::neuter;
        }
    }
    return DeGrammarSynthesizer::makeLookupKey(countValue, genderValue, caseValue);
}

} // namespace inflection::grammar::synthesis
