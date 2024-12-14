/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NbGrammarSynthesizer_NbDisplayFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dictionary/Inflector_InflectionPattern.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

NbGrammarSynthesizer_NbDisplayFunction::NbGrammarSynthesizer_NbDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::NORWEGIAN())))
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::NORWEGIAN()))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::NORWEGIAN()))
    , genderLookupFunction(::morphuntion::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()})
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->caseFeature = model.getFeature(GrammemeConstants::CASE);
    this->definitenessFeature = model.getFeature(GrammemeConstants::DEFINITENESS);
    this->posFeature = model.getFeature(GrammemeConstants::POS);
}

NbGrammarSynthesizer_NbDisplayFunction::~NbGrammarSynthesizer_NbDisplayFunction()
{
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectString(const ::std::u16string& word, int64_t existingWordGrammemes, const ::std::u16string& count, const ::std::u16string& definiteness, const ::std::u16string& targetGender, bool targetIsANoun) const
{
    if (!targetIsANoun) {
        return inflectAdjective(word, definiteness, targetGender, count);
    }
    std::vector<morphuntion::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    inflector.getInflectionPatternsForWord(word, inflectionPatterns);
    if (!inflectionPatterns.empty()) {
        ::std::vector<::std::u16string> grammemes;
        for (const auto& grammeme : {count, definiteness, targetGender}) {
            if (!grammeme.empty()) {
                grammemes.emplace_back(grammeme);
            }
        }
        int64_t targetGrammemes = dictionary.getBinaryProperties(grammemes);
        for (const auto& inflectionPattern : inflectionPatterns) {
            auto result(inflectionPattern.reinflect(existingWordGrammemes, targetGrammemes, word));
            if (!result.empty()) {
                return result;
            }
        }
    }
    return word;
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * NbGrammarSynthesizer_NbDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (!constraints.empty()) {
        int64_t wordType = 0;
        if (dictionary.getCombinedBinaryType(&wordType, displayString) != nullptr) {
            displayString = inflectWord(constraints, displayString, wordType);
        } else {
            ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            displayString = inflectTokenChain(constraints, *tokenChain);
        }
    }
    return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints);
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectTokenChain(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *this);
}

::std::vector<::std::u16string> NbGrammarSynthesizer_NbDisplayFunction::inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto indexOfLastSignificant = indexesOfSignificant.back();
    int64_t significantWordType = 0;
    auto significantWord((*npc(tokens))[indexOfLastSignificant]);
    if (dictionary.getCombinedBinaryType(&significantWordType, significantWord) == nullptr) {
        ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
        if (GrammemeConstants::CASE_GENITIVE() == caseString) {
            npc(tokens)->at(indexOfLastSignificant) = inflectGenitive((*tokens)[indexOfLastSignificant]);
            return *npc(tokens);
        }
        return {};
    }
    if (indexesOfSignificant.size() == 2) {
        auto word0 = (*tokens)[indexesOfSignificant[0]];
        auto word1 = (*tokens)[indexesOfSignificant[1]];
        int64_t wordType0 = 0;
        dictionary.getCombinedBinaryType(&wordType0, word0);
        int64_t wordType1 = 0;
        dictionary.getCombinedBinaryType(&wordType1, word1);
        if ((wordType0 & (dictionaryAdjective | dictionaryNoun)) == dictionaryAdjective && (wordType1 & dictionaryNoun) != 0) {
            npc(tokens)->at(indexesOfSignificant[0]) = inflectWord(constraints, word0, wordType0, word1, false);
            npc(tokens)->at(indexesOfSignificant[1]) = inflectWord(constraints, word1, wordType1, word1, true);
        } else {
            npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, significantWord, significantWordType);
        }
    } else {
        npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, significantWord, significantWordType);
    }
    return *npc(tokens);
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString, int64_t displayStringGrammemes) const
{
    return inflectWord(constraints, displayString, displayStringGrammemes, displayString, true);
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, int64_t attributeDisplayStringGrammemes, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    ::std::u16string definitenessString(GrammarSynthesizerUtil::getFeatureValue(constraints, definitenessFeature));
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    int64_t headBinaryType = 0;
    if (attributeDisplayString == headDisplayString) {
        headBinaryType = attributeDisplayStringGrammemes;
    }
    else {
        dictionary.getCombinedBinaryType(&headBinaryType, headDisplayString);
    }
    auto isAdjective = (GrammemeConstants::POS_ADJECTIVE() == GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature)) || ((headBinaryType & dictionaryAdjective) != 0 && (headBinaryType & dictionaryNoun) == 0);
    auto genderString(GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
    if (genderString.empty()) {
        genderString = genderLookupFunction.determine(headDisplayString);
    }
    auto inflectedString(inflectString(attributeDisplayString, attributeDisplayStringGrammemes, countString, definitenessString, genderString, !isAdjective && isSuspectedToBeANoun));
    if (GrammemeConstants::CASE_GENITIVE() == caseString && isSuspectedToBeANoun && inflectedString.length() > 1) {
        inflectedString = inflectGenitive(inflectedString);
    }
    return inflectedString;
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectGenitive(const ::std::u16string& string)
{
    auto inflectedString(string);
    auto suffix = inflectedString[inflectedString.length() - 1];
    if (suffix == u's' || suffix == u'z' || suffix == u'x') {
        inflectedString.append(u"’");
    }
    else {
        inflectedString.append(u"s");
    }
    return inflectedString;
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount)
{
    auto length = lemma.length();
    if (length < 1) {
        return lemma;
    }
    auto charAtM1 = lemma[length - 1];
    auto endsInConsonant = !(charAtM1 == u'i' || charAtM1 == u'\u00e5' || charAtM1 == u'e' || charAtM1 == u'a' || charAtM1 == u'u' || charAtM1 == u'o' || charAtM1 == u'y' || charAtM1 == u'\u00f8' || charAtM1 == u'\u00e6');
    if (GrammemeConstants::NUMBER_PLURAL() == targetCount || GrammemeConstants::DEFINITENESS_DEFINITE() == targetDefiniteness) {
        if (length > 3) {
            auto charAtM3 = lemma[length - 3];
            if (lemma[length - 2] == u'e' && (charAtM1 == u'n' || charAtM1 == u'l' || charAtM1 == u'r') && charAtM3 == lemma[length - 4] && !(charAtM3 == u'i' || charAtM3 == u'\u00e5' || charAtM3 == u'e' || charAtM3 == u'a' || charAtM3 == u'u' || charAtM3 == u'o' || charAtM3 == u'y' || charAtM3 == u'\u00f8' || charAtM3 == u'\u00e6')) {
                return lemma.substr(0, length - 3) + charAtM1 + u'e';
            }
        }
        if (length > 3 && lemma[length - 2] == u'e' && (charAtM1 == u'n' || charAtM1 == u'l' || charAtM1 == u'r')) {
            return lemma.substr(0, length - 2) + charAtM1 + u'e';
        }
        if (endsInConsonant) {
            return lemma + u"e";
        }
    }
    if (GrammemeConstants::NUMBER_SINGULAR() == targetCount && GrammemeConstants::GENDER_NEUTER() == targetGender && endsInConsonant) {
        if (length > 2) {
            auto charAtM2 = lemma[length - 2];
            if ((charAtM2 == u's' && charAtM1 == u'k') || (charAtM2 == u'i' && charAtM1 == u'g') || (charAtM2 == u'e' && charAtM1 == u't')) {
                return lemma;
            }
            auto isM2Cons = !(charAtM2 == u'i' || charAtM2 == u'\u00e5' || charAtM2 == u'e' || charAtM2 == u'a' || charAtM2 == u'u' || charAtM2 == u'o' || charAtM2 == u'y' || charAtM2 == u'\u00f8' || charAtM2 == u'\u00e6');
            if (isM2Cons && (charAtM1 == u't' || charAtM1 == u'd')) {
                return lemma;
            }
            if (isM2Cons && (charAtM1 == charAtM2)) {
                return lemma.substr(0, length - 1) + u't';
            }
        }
        return lemma + u"t";
    }
    return lemma;
}

} // namespace morphuntion::grammar::synthesis