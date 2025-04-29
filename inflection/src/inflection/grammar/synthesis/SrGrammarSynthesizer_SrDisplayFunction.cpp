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
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::SERBIAN()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SERBIAN()))
    , dictionaryInflector(::inflection::util::LocaleUtils::SERBIAN(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()}
    }, {}, true)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
}

SrGrammarSynthesizer_SrDisplayFunction::~SrGrammarSynthesizer_SrDisplayFunction()
{
}

void printString16(const std::u16string& input)
{
    // Convert std::u16string to std::string
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string utf8_str = convert.to_bytes(input);

    // Output the converted string
    std::cout << "<<<< Display string: " << utf8_str << " >>>>>>" << std::endl;
}

::std::u16string SrGrammarSynthesizer_SrDisplayFunction::inflectString(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& lemma) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);

    ::std::u16string inflection;

    // The nominative/caseless is unmarked in the patterns so we need to do something like this
    ::std::vector<::std::u16string> string_constraints;
    if (!countString.empty()) {
        string_constraints.emplace_back(countString);
    }
    if (!caseString.empty() && caseString != GrammemeConstants::CASE_NOMINATIVE()) {
        string_constraints.emplace_back(caseString);
    }
    if (!genderString.empty()) {
        string_constraints.emplace_back(genderString);
    }
    // The nominative/caseless is unmarked in the patterns, so we need to do something like this
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, lemma);

    const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> disambiguationConstraints;
    const auto dismbiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(disambiguationConstraints, {&posFeature}));
    auto inflectionResult = dictionaryInflector.inflect(lemma, wordGrammemes, string_constraints, dismbiguationGrammemeValues);
    if (inflectionResult) {
        inflection = *inflectionResult;
    }

    if (inflection.empty()) {
        printString16(lemma);
        return lemma;
    }

    printString16(inflection);
    return inflection;
}

::inflection::dialog::DisplayValue * SrGrammarSynthesizer_SrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /* enableInflectionGuess */) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        std::cout << "######### 1 " << std::endl;
        displayString = displayData.getValues()[0].getDisplayString();
        printString16(displayString);
    }
    if (displayString.empty()) {
        std::cout << "######### 2 " << std::endl;
        return nullptr;
    }
    if (dictionary.isKnownWord(displayString)) {
        std::cout << "######### 3 isKnownWord" << std::endl;
        displayString = inflectString(constraints, displayString);
    }
    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

} // namespace inflection::grammar::synthesis
