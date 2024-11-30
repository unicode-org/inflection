/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer_DaDisplayFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/npc.hpp>
#include <algorithm>
#include <memory>
#include <unicode/uchar.h>

namespace morphuntion::grammar::synthesis {

DaGrammarSynthesizer_DaDisplayFunction::DaGrammarSynthesizer_DaDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::DANISH())))
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::DANISH()))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::DANISH()))
    , genderLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()})
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->caseFeature = model.getFeature(GrammemeConstants::CASE);
    this->definitenessFeature = model.getFeature(GrammemeConstants::DEFINITENESS);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->posFeature = model.getFeature(GrammemeConstants::POS);
}

DaGrammarSynthesizer_DaDisplayFunction::~DaGrammarSynthesizer_DaDisplayFunction()
{

}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectString(const ::std::u16string& lemma, const ::std::vector<::std::u16string>& inflectionType, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const
{
    ::std::u16string inflection;
    ::std::u16string count(targetCount);
    ::std::u16string definiteness(targetDefiniteness);

    if (count.empty()) {
        count = GrammemeConstants::NUMBER_SINGULAR();
    }
    if (definiteness.empty()) {
        definiteness = GrammemeConstants::DEFINITENESS_INDEFINITE();
    }
    if (!targetIsANoun) {
        return inflectAdjective(lemma, definiteness, targetGender, count);
    }
    for (const auto& patternName : inflectionType) {
        auto inflectionPattern = inflector.getInflectionPatternByName(patternName);
        if (!inflectionPattern) {
            ::morphuntion::util::Logger::warn(u"Lemma " + lemma + u" has inflection=" + patternName + u" but that pattern was not found");
        } else {
            if (targetIsANoun != inflectionPattern->containsPartsOfSpeech(GrammemeConstants::POS_NOUN())) {
                continue;
            }
            ::std::vector<::std::u16string> constraints;
            constraints.emplace_back(targetCount);
            if (!targetDefiniteness.empty()) {
                constraints.emplace_back(targetDefiniteness);
            }
            if (!targetCase.empty() && targetCase != GrammemeConstants::CASE_NOMINATIVE()) {
                constraints.emplace_back(targetCase);
            }
            if (!targetGender.empty()) {
                constraints.emplace_back(targetGender);
            }
            int64_t wordGrammemes = 0;
            dictionary.getCombinedBinaryType(&wordGrammemes, lemma);
            int64_t toConstraintGrammemes = 0;
            dictionary.getBinaryProperties(&toConstraintGrammemes, constraints);
            inflection = inflectionPattern->reinflect(wordGrammemes, toConstraintGrammemes, lemma);
        }
    }
    return inflection;
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * DaGrammarSynthesizer_DaDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (!constraints.empty()) {
        if (morphuntion::util::StringViewUtils::endsWith(displayString, u".")) {
            // These abbreviations don't change definiteness nor change grammatical number.
            displayString = makeGenitiveWhenRequested(constraints, displayString);
        }
        else if (dictionary.isKnownWord(displayString)) {
            displayString = inflectWord(constraints, displayString);
        }
        else {
            ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            displayString = inflectTokenChain(constraints, *npc(tokenChain.get()));
        }
    }
    return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints);
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectTokenChain(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *this);
}

::std::vector<::std::u16string> DaGrammarSynthesizer_DaDisplayFunction::inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto indexOfLastSignificant = indexesOfSignificant.back();
    if (!dictionary.isKnownWord(npc(tokens)->at(indexOfLastSignificant))) {
        auto lastSignificantToken(npc(tokens)->at(indexOfLastSignificant));
        auto result(makeGenitiveWhenRequested(constraints, lastSignificantToken));
        if (result != lastSignificantToken) {
            npc(tokens)->at(indexOfLastSignificant) = result;
            return *npc(tokens);
        }
        return {};
    }
    if (indexesOfSignificant.size() == 2) {
        const auto& word0 = npc(tokens)->at(indexesOfSignificant[0]);
        const auto& word1 = npc(tokens)->at(indexesOfSignificant[1]);
        int64_t wordType0 = 0;
        int64_t wordType1 = 0;
        auto res0 = dictionary.getCombinedBinaryType(&wordType0, word0);
        auto res1 = dictionary.getCombinedBinaryType(&wordType1, word1);

        if (res0 == nullptr || res1 == nullptr || ((wordType0 & dictionaryAdjective) != 0 && (wordType0 & dictionaryNoun) == 0 && (wordType1 & dictionaryNoun) != 0)) {
            npc(tokens)->at(indexesOfSignificant[0]) = inflectWord(constraints, word0, word1, false, false);
            npc(tokens)->at(indexesOfSignificant[1]) = inflectWord(constraints, word1, word1, true, true);

        } else {
            npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, npc(tokens)->at(indexesOfSignificant[1]));
        }
    } else {
        npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, npc(tokens)->at(indexOfLastSignificant));
    }
    return *npc(tokens);
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const
{
    return inflectWord(constraints, displayString, displayString, true, false);
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectWord(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString, bool isSuspectedToBeANoun, bool isHeadWordWithAttribute) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    ::std::u16string definitenessString;
    if (isHeadWordWithAttribute) {
        definitenessString = GrammemeConstants::DEFINITENESS_INDEFINITE();
    } else {
        definitenessString = GrammarSynthesizerUtil::getFeatureValue(constraints, definitenessFeature);
    }
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    int64_t binaryType = 0;
    dictionary.getCombinedBinaryType(&binaryType, headDisplayString);
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);
    if (genderString.empty()) {
        genderString = genderLookupFunction.determine(headDisplayString);
    }
    ::std::u16string posString(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));
    auto isAdjective = GrammemeConstants::POS_ADJECTIVE() == posString || (((binaryType & dictionaryAdjective) != 0) && ((binaryType & dictionaryNoun) == 0));
    auto inflectionType = dictionary.getPropertyValues(attributeDisplayString, u"inflection");
    auto inflectedString = inflectString(attributeDisplayString, inflectionType, countString, definitenessString, caseString, genderString, !isAdjective && isSuspectedToBeANoun);
    if (isSuspectedToBeANoun && inflectedString.empty()) {
        return makeGenitiveWhenRequested(constraints, attributeDisplayString);
    }
    if (inflectedString.empty()) {
        return attributeDisplayString;
    }
    std::replace( inflectedString.begin(), inflectedString.end(), u'\'', u'’');
    return inflectedString;
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::makeGenitiveWhenRequested(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const
{
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    if (GrammemeConstants::CASE_GENITIVE() == caseString && displayString.length() > 1) {
        // See https://sproget.dk/raad-og-regler/typiske-problemer/genitiv-ejefald/genitiv/ for more information
        auto suffix = u_tolower(displayString.back());
        if (suffix == u's' || suffix == u'z' || suffix == u'x') {
            return displayString + u'’';
        }
        if ((morphuntion::util::StringViewUtils::isAllUpperCase(displayString) && !(bool)u_ispunct(suffix))
            || (bool)u_isdigit(suffix)
            || morphuntion::util::StringViewUtils::endsWith(displayString, u".dk"))
        {
            return displayString + u"’s";
        }
        else {
            return displayString + u's';
        }
    }
    return displayString;
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount)
{
    auto length = lemma.length();
    if (length < 1) {
        return lemma;
    }
    auto charAtM1 = lemma[length - 1];
    auto endsInIAA = charAtM1 == u'i' || charAtM1 == u'\u00e5';
    auto endsInConsonant = !(endsInIAA || charAtM1 == u'e' || charAtM1 == u'a' || charAtM1 == u'u' || charAtM1 == u'o' || charAtM1 == u'y' || charAtM1 == u'\u00f8' || charAtM1 == u'\u00e6');
    if (GrammemeConstants::NUMBER_PLURAL() == targetCount || GrammemeConstants::DEFINITENESS_DEFINITE() == targetDefiniteness) {
        if (length > 4) {
            auto charAtM3 = lemma[length - 3];
            if (lemma[length - 2] == u'e' && (charAtM1 == u'n' || charAtM1 == u'l' || charAtM1 == u'r') && charAtM3 == lemma[length - 4] && !(charAtM3 == u'i' || charAtM3 == u'\u00e5' || charAtM3 == u'e' || charAtM3 == u'a' || charAtM3 == u'u' || charAtM3 == u'o' || charAtM3 == u'y' || charAtM3 == u'\u00f8' || charAtM3 == u'\u00e6')) {
                return lemma.substr(0, length - 3) + charAtM1 + u'e';
            }
        }
        if (endsInConsonant) {
            return lemma + u"e";
        }
    }
    if (GrammemeConstants::NUMBER_SINGULAR() == targetCount
        && GrammemeConstants::GENDER_NEUTER() == targetGender && (endsInIAA || endsInConsonant) && charAtM1 != u't')
    {
        return lemma + u"t";
    }
    return lemma;
}



} // namespace morphuntion::grammar::synthesis
