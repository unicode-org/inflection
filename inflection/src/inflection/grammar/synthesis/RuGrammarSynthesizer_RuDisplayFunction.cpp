/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_RuDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <iterator>
#include <memory>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_RuDisplayFunction::RuGrammarSynthesizer_RuDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::RUSSIAN())))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , animacyFeature(*npc(model.getFeature(GrammemeConstants::ANIMACY)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
{
    nonRussianCyrillicChars.addAll(inflection::lang::StringFilterUtil::CYRILLIC_SCRIPT());
    nonRussianCyrillicChars.removeAll(inflection::lang::StringFilterUtil::getExemplarCharacters(::inflection::util::LocaleUtils::RUSSIAN()));
    nonRussianCyrillicChars.freeze();
}

RuGrammarSynthesizer_RuDisplayFunction::~RuGrammarSynthesizer_RuDisplayFunction()
{
}

static ::std::u16string getFeatureValue(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature& semanticFeature)
{
    auto result = constraints.find(semanticFeature);
    if (result != constraints.end()) {
        return result->second;
    }
    return {};
}

constexpr int32_t MAXIMUM_WORDS_MODEL_INPUT = 6;

::std::u16string RuGrammarSynthesizer_RuDisplayFunction::inflectWithML(const ::std::u16string& /*inputString*/, const ::inflection::tokenizer::TokenChain& /*words*/, ::std::u16string_view /*caseString*/, ::std::u16string_view /*countString*/, ::std::u16string_view /*genderString*/, ::std::u16string_view /*animacyString*/) const
{
    ::std::u16string inflectionResult;
    // TODO implement code here
    return inflectionResult;
}

::inflection::dialog::DisplayValue * RuGrammarSynthesizer_RuDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    if (displayData.getValues().empty()) {
        return nullptr;
    }
    
    auto firstDisplayValue(displayData.getValues()[0]);
    const auto& inputString(firstDisplayValue.getDisplayString());

    // Constraints are empty or no inflectable characters in input or contains a non-Russian cyrillic character -> return value as is
    if (constraints.empty() || !inflection::util::UnicodeSetUtils::containsSome(russianMorphology.inflectableChars, inputString) || inflection::util::UnicodeSetUtils::containsSome(nonRussianCyrillicChars, inputString)) {
        return new inflection::dialog::DisplayValue(firstDisplayValue);
    }
    
    // Start: CONSTRAINT RETRIEVAL
    auto caseString(getFeatureValue(constraints, caseFeature));
    auto countString(getFeatureValue(constraints, countFeature));
    auto genderString(getFeatureValue(constraints, genderFeature));
    auto animacyString(getFeatureValue(constraints, animacyFeature));
    auto posString(getFeatureValue(constraints, posFeature));
    
    if (caseString.empty()) {
        caseString = GrammemeConstants::CASE_NOMINATIVE();
    }
    else if (caseString == GrammemeConstants::CASE_ABLATIVE()) {
        // Remap deprecated grammeme
        caseString = GrammemeConstants::CASE_INSTRUMENTAL();
    }
    else if (caseString == GrammemeConstants::CASE_LOCATIVE()) {
        // Remap deprecated grammeme
        caseString = GrammemeConstants::CASE_PREPOSITIONAL();
    }
    // End

    ::std::u16string inflectionResult;
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> words(npc(npc(tokenizer.get())->createTokenChain(inputString)));
    
    // If stripped input contains a character from avoid set then we can't use ML model
    bool cantFallBackToML = !enableInflectionGuess;
    for (const auto &wordIter : *words) {
        if (wordIter.isSignificant() && !russianMorphology.inflectableChars.containsAllCodePoints(wordIter.getValue())) {
            cantFallBackToML = true;
            break;
        }
    }
    if (words->getWordCount() > MAXIMUM_WORDS_MODEL_INPUT) {
        cantFallBackToML = true;
    }

    //Start: PHRASE ML FALLBACK - Use dictionary to inflect only if input is a single word or we can't fall back to ML
    bool inflectByML = words->getWordCount() != 1 && !cantFallBackToML;

    // Start: DICTIONARY INFLECT - Tries to inflect using dictionary, fall back to machine learning model if unable to do so
    auto wordIter = words->end();
    while (!inflectByML && wordIter != words->begin()) {
        --wordIter;
        //This ensures we can look into previous token
        if (wordIter == words->begin()) {
            break;
        }
        auto word(wordIter->getValue());

        // If preceding token's a hyphen then merge preceding word with current word including hyphen
        if (npc(wordIter->getPrevious())->getValue() == u"-") {
            word.insert(0, (--wordIter)->getValue());
            // Only go backward when we are not at the beginning token
            if (wordIter != words->begin()) {
                word.insert(0, (--wordIter)->getValue());
            }
        }
        auto dictionaryResult(russianMorphology.inflectUsingDictionary(word, caseString, countString, genderString, animacyString, posString));
        if (dictionaryResult) {
            inflectionResult.insert(0, *dictionaryResult);
        } else if (cantFallBackToML) {
            inflectionResult.insert(0, word);
        } else {
            inflectByML = true;
        }
    }
    // End
    // Start: ML INFLECT - Entire phrase is fed to an ML model to inflect with given constraints
    if (inflectByML) {
        inflectionResult = inflectWithML(inputString, *words, caseString, countString, genderString, animacyString);
        if (inflectionResult.empty()) {
            // Failed to inflect
            return new ::inflection::dialog::DisplayValue(firstDisplayValue);
        }
    }
    // End

    inflectionResult[0] = inputString[0]; // Keep the letter case of the first letter.
    
    return new ::inflection::dialog::DisplayValue(inflectionResult, constraints);
}

} // namespace inflection::grammar::synthesis
