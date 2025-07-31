/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_SvDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

SvGrammarSynthesizer_SvDisplayFunction::SvGrammarSynthesizer_SvDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::SWEDISH())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::SWEDISH()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SWEDISH()))
    , dictionaryInflector(::inflection::util::LocaleUtils::SWEDISH(), {
        {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}
        }, {}, true)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
}

SvGrammarSynthesizer_SvDisplayFunction::~SvGrammarSynthesizer_SvDisplayFunction()
{
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectString(const ::std::u16string& lemma, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const
{
    ::std::u16string inflection;

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

    constexpr std::vector<std::u16string> disambiguationGrammemeValues;
    auto inflectionResult = dictionaryInflector.inflect(lemma, wordGrammemes, constraints, disambiguationGrammemeValues);
    if (inflectionResult) {
        inflection = *inflectionResult;
    }

    if (inflection.empty()) {
        return lemma;
    }
    return inflection;
}

::inflection::dialog::DisplayValue * SvGrammarSynthesizer_SvDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
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
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        displayString = inflectTokenChain(constraints, *npc(tokenChain.get()));
    }
    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectTokenChain(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *npc(this));
}

::std::vector<::std::u16string> SvGrammarSynthesizer_SvDisplayFunction::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
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

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const
{
    return inflectWord(constraints, displayString, displayString, true);
}

::std::u16string SvGrammarSynthesizer_SvDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool targetIsNoun) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    ::std::u16string definitenessString(GrammarSynthesizerUtil::getFeatureValue(constraints, definitenessFeature));
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);
    if (genderString.empty() && GrammemeConstants::NUMBER_SINGULAR() == countString) {
        genderString = genderLookupFunction.determine(headDisplayString);
    }
    auto inflectedString = inflectString(attributeDisplayString, countString, definitenessString, caseString, genderString, targetIsNoun);
    if (GrammemeConstants::CASE_GENITIVE() == caseString && inflectedString.length() > 1) {
        auto suffix = inflectedString.back();
        if (!(suffix == u's' || suffix == u'z' || suffix == u'x')) {
            inflectedString += u"s";
        }
    }
    return inflectedString;
}

} // namespace inflection::grammar::synthesis
