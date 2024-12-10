/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_SvDisplayFunction.hpp>

#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

SvGrammarSynthesizer_SvDisplayFunction::SvGrammarSynthesizer_SvDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::SWEDISH())))
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::SWEDISH()))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::SWEDISH()))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
}

SvGrammarSynthesizer_SvDisplayFunction::~SvGrammarSynthesizer_SvDisplayFunction()
{
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectString(const ::std::u16string& lemma, const ::std::vector<::std::u16string>& inflectionType, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const
{
    ::std::u16string inflection;
    for (const auto& patternName : inflectionType) {
        auto inflectionPattern = inflector.getInflectionPatternByName(patternName);
        if (!inflectionPattern) {
            ::morphuntion::util::Logger::warn(u"Lemma " + lemma + u" has inflection=" + patternName + u" but that pattern was not found");
        } else {
            if (targetIsANoun != inflectionPattern->containsPartsOfSpeech(GrammemeConstants::POS_NOUN())) {
                continue;
            }
            // The nominative/caseless is unmarked in the patterns so we need to do something like this
            ::std::vector<::std::u16string> constraints;
            constraints.emplace_back(targetCount);
            if (!targetDefiniteness.empty() && (targetIsANoun || targetCount == GrammemeConstants::NUMBER_SINGULAR())) {
                constraints.emplace_back(targetDefiniteness);
            }
            if (!targetCase.empty() && targetCase != GrammemeConstants::CASE_NOMINATIVE()) {
                constraints.emplace_back(targetCase);
            }
            if (!targetGender.empty()) {
                constraints.emplace_back(targetGender);
            }
            // The nominative/caseless is unmarked in the patterns, so we need to do something like this
            int64_t wordGrammemes = 0;
            dictionary.getCombinedBinaryType(&wordGrammemes, lemma);
            int64_t toConstraintGrammemes = 0;
            dictionary.getBinaryProperties(&toConstraintGrammemes, constraints);
            inflection = inflectionPattern->reinflect(wordGrammemes, toConstraintGrammemes, lemma);
        }
    }
    if (inflection.empty()) {
        return lemma;
    }
    return inflection;
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * SvGrammarSynthesizer_SvDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (dictionary.isKnownWord(displayString)) {
        displayString = inflectWord(constraints, displayString);
    } else {
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        displayString = inflectTokenChain(constraints, *npc(tokenChain.get()));
    }
    return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints);
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectTokenChain(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *npc(this));
}

::std::vector<::std::u16string> SvGrammarSynthesizer_SvDisplayFunction::inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto indexOfLastSignificant = indexesOfSignificant.back();
    if (!dictionary.isKnownWord((*tokens)[indexOfLastSignificant])) {
        return {};
    }
    if (indexesOfSignificant.size() == 2) {
        auto word0 = (*tokens)[indexesOfSignificant[0]];
        auto word1 = (*tokens)[indexesOfSignificant[1]];
        int64_t wordType0 = 0;
        dictionary.getCombinedBinaryType(&wordType0, word0);
        int64_t wordType1 = 0;
        dictionary.getCombinedBinaryType(&wordType1, word1);
        if (wordType0 == 0 || wordType1 == 0 || ((wordType0 & dictionaryAdjective) != 0 && (wordType0 & dictionaryNoun) == 0 && (wordType1 & dictionaryNoun) != 0)) {
            npc(tokens)->at(indexesOfSignificant[0]) = inflectWord(constraints, word0, word1, false);
        }
        npc(tokens)->at(indexesOfSignificant[1]) = inflectWord(constraints, word1);
    } else {
        npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, (*tokens)[indexOfLastSignificant]);
    }
    return *tokens;
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const
{
    return inflectWord(constraints, displayString, displayString, true);
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool targetIsNoun) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, &countFeature));
    ::std::u16string definitenessString(GrammarSynthesizerUtil::getFeatureValue(constraints, &definitenessFeature));
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, &caseFeature));
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, &genderFeature);
    if (genderString.empty()) {
        genderString = genderLookupFunction.determine(headDisplayString);
    }
    auto inflectionType = dictionary.getPropertyValues(attributeDisplayString, u"inflection");
    auto inflectedString = inflectString(attributeDisplayString, inflectionType, countString, definitenessString, caseString, genderString, targetIsNoun);
    if (GrammemeConstants::CASE_GENITIVE() == caseString && inflectedString.length() > 1) {
        auto suffix = inflectedString.back();
        if (!(suffix == u's' || suffix == u'z' || suffix == u'x')) {
            inflectedString += u"s";
        }
    }
    return inflectedString;
}

} // namespace morphuntion::grammar::synthesis
