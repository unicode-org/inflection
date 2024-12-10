/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include "morphuntion/tokenizer/TokenChain.hpp"
#include "morphuntion/tokenizer/Tokenizer.hpp"
#include "morphuntion/tokenizer/TokenizerFactory.hpp"
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

DeGrammarSynthesizer_ArticleLookupFunction::DeGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::map<int32_t, ::std::u16string_view>& articleMap)
    : super(model, includeSemanticValue, true)
    , dictionary(*::morphuntion::util::Validate::notNull(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::GERMAN())))
    , caseFeature(::morphuntion::util::Validate::notNull(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(::morphuntion::util::Validate::notNull(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(::morphuntion::util::Validate::notNull(model.getFeature(GrammemeConstants::GENDER)))
    , articleMap(articleMap)
    , tokenizer(::morphuntion::util::Validate::notNull(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::GERMAN())))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {u"neuter"}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {u"singular"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    dictionaryCount = dictionarySingular | dictionaryPlural;
}

DeGrammarSynthesizer_ArticleLookupFunction::DeGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const DeGrammarSynthesizer_ArticleLookupFunction& other)
    : DeGrammarSynthesizer_ArticleLookupFunction(model, includeSemanticValue, other.articleMap)
{
}

DeGrammarSynthesizer_ArticleLookupFunction::~DeGrammarSynthesizer_ArticleLookupFunction()
{

}

morphuntion::dialog::SpeakableString* DeGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    auto lookupKey = getArticleKey(&displayValue);
    auto preposition = articleMap.find(lookupKey.value);
    if (preposition == articleMap.end()) {
        return createPreposition(displayValue, ::std::u16string());
    }
    return createPreposition(displayValue, ::std::u16string(preposition->second));
}

DeGrammarSynthesizer::LookupKey DeGrammarSynthesizer_ArticleLookupFunction::getArticleKey(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* displayValue) const
{
    auto caseValue = DeGrammarSynthesizer::getCase(npc(displayValue)->getFeatureValue(*npc(caseFeature)));
    auto countValue = DeGrammarSynthesizer::getCount(npc(displayValue)->getFeatureValue(*npc(countFeature)));
    auto genderValue = DeGrammarSynthesizer::getGender(npc(displayValue)->getFeatureValue(*npc(genderFeature)));
    int64_t phraseType = 0;
    if (countValue == DeGrammarSynthesizer::Count::undefined || genderValue == DeGrammarSynthesizer::Gender::undefined) {
        auto displayString = npc(displayValue)->getDisplayString();
        if (dictionary.getCombinedBinaryType(&phraseType, displayString) == nullptr) {
            ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> words(npc(tokenizer.get())->createTokenChain(displayString));
            if (words->getWordCount() > 1) {
                dictionary.getCombinedBinaryType(&phraseType, npc(npc(words->getEnd())->getPrevious())->getValue());
            }
        }
    }
    if (caseValue == DeGrammarSynthesizer::Case::undefined) {
        caseValue = DeGrammarSynthesizer::Case::nominative;
    }
    if (countValue == DeGrammarSynthesizer::Count::undefined) {
        if (phraseType != 0 && (phraseType & dictionaryCount) == dictionaryPlural) {
            countValue = DeGrammarSynthesizer::Count::plural;
        }
        else {
            countValue = DeGrammarSynthesizer::Count::singular;
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

} // namespace morphuntion::grammar::synthesis
