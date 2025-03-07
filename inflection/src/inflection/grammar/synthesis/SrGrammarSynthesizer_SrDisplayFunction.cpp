/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <inflection/grammar/synthesis/SrGrammarSynthesizer_SrDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <iterator>
// Delete later.
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
//
#include <memory>

namespace inflection::grammar::synthesis {

SrGrammarSynthesizer_SrDisplayFunction::SrGrammarSynthesizer_SrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::SERBIAN())))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SERBIAN()))
    , dictionaryInflector(::inflection::util::LocaleUtils::SERBIAN(),{
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

static ::std::u16string getFeatureValue(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature semanticFeature)
{
    auto result = constraints.find(semanticFeature);
    if (result != constraints.end()) {
        return result->second;
    }
    return {};
}

void printString16(std::u16string& input)
{
    // Convert std::u16string to std::string
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string utf8_str = convert.to_bytes(input);

    // Output the converted string
    std::cout << "<<<< Display string: " << utf8_str << " >>>>>>" << std::endl;
}

::inflection::dialog::DisplayValue * SrGrammarSynthesizer_SrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        std::cout << "################## 1";
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    printString16(displayString);
    if (displayString.empty()) {
        std::cout << "################## 2";
        return nullptr;
    }

    // Start: CONSTRAINT RETRIEVAL
    auto caseString(getFeatureValue(constraints, caseFeature));

    // To make compiler quiet about unused variable.
    if (enableInflectionGuess)
        return new ::inflection::dialog::DisplayValue(*displayValue);

    // TODO Implement the rest
    std::cout << "################## 3";
    return new ::inflection::dialog::DisplayValue(*displayValue);
}

} // namespace inflection::grammar::synthesis
