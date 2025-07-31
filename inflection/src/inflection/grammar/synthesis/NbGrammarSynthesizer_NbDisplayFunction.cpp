/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NbGrammarSynthesizer_NbDisplayFunction.hpp>

#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

NbGrammarSynthesizer_NbDisplayFunction::NbGrammarSynthesizer_NbDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::NORWEGIAN())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::NORWEGIAN()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::NORWEGIAN()))
    , genderLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()})
    , dictionaryInflector(::inflection::util::LocaleUtils::NORWEGIAN(), {
        {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}
        }, {}, true)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
}

NbGrammarSynthesizer_NbDisplayFunction::~NbGrammarSynthesizer_NbDisplayFunction()
{
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectNoun(const ::std::u16string& word, int64_t existingWordGrammemes, const ::std::u16string& count, const ::std::u16string& definiteness, const ::std::u16string& targetGender) const
{
    ::std::vector<::std::u16string> constraints;
    if (!count.empty()) {
        constraints.emplace_back(count);
    }
    if (!definiteness.empty()) {
        constraints.emplace_back(definiteness);
    }
    if (GrammemeConstants::NUMBER_PLURAL() != count && !targetGender.empty()) {
        constraints.emplace_back(targetGender);
    }

    constexpr std::vector<std::u16string> disambiguationGrammemeValues;
    return dictionaryInflector.inflect(word, existingWordGrammemes, constraints, disambiguationGrammemeValues).value_or(word);
}

::inflection::dialog::DisplayValue * NbGrammarSynthesizer_NbDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
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
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            displayString = inflectTokenChain(constraints, *tokenChain);
        }
    }
    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectTokenChain(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *this);
}

::std::vector<::std::u16string> NbGrammarSynthesizer_NbDisplayFunction::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
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

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString, int64_t displayStringGrammemes) const
{
    return inflectWord(constraints, displayString, displayStringGrammemes, displayString, true);
}

::std::u16string NbGrammarSynthesizer_NbDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, int64_t attributeDisplayStringGrammemes, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
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

    std::u16string inflectedString;
    if (isAdjective || !isSuspectedToBeANoun) {
        if (genderString.empty()) {
            genderString = genderLookupFunction.determine(headDisplayString);
        }
        inflectedString = inflectAdjective(attributeDisplayString, definitenessString, genderString, countString);
    }
    else {
        inflectedString = inflectNoun(attributeDisplayString, attributeDisplayStringGrammemes, countString, definitenessString, genderString);
    }

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

} // namespace inflection::grammar::synthesis
