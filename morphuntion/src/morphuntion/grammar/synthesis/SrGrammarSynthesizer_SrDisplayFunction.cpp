/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <morphuntion/grammar/synthesis/SrGrammarSynthesizer_SrDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

SrGrammarSynthesizer_SrDisplayFunction::SrGrammarSynthesizer_SrDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::FRENCH())))
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , partOfSpeechFeature(model.getFeature(GrammemeConstants::POS))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::FRENCH()))
    , dictionaryInflector(::morphuntion::util::LocaleUtils::SERBIAN(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()}
    }, {}, true)
{
}

SrGrammarSynthesizer_SrDisplayFunction::~SrGrammarSynthesizer_SrDisplayFunction()
{
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * SrGrammarSynthesizer_SrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }

    // TODO Implement the rest
}

} // namespace morphuntion::grammar::synthesis
