/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer_PtDisplayFunction.hpp>

#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <memory>
#include <vector>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::morphuntion::exception::ClassCastException();
    return t;
}

namespace morphuntion::grammar::synthesis {

PtGrammarSynthesizer_PtDisplayFunction::PtGrammarSynthesizer_PtDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::PORTUGUESE())))
    , definiteArticleLookupFunction(model, PtGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const PtGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(PtGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, PtGrammarSynthesizer::ARTICLE_INDEFINITE, *npc(java_cast<const PtGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(PtGrammarSynthesizer::ARTICLE_INDEFINITE))))))
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, PtGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, PtGrammarSynthesizer::ARTICLE_INDEFINITE)
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::PORTUGUESE()))
    , dictionaryInflector(::morphuntion::util::LocaleUtils::PORTUGUESE(),{
        {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
        {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    })
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {u"singular"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryVerb, {u"verb"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdverb, {u"adverb"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdposition, {u"adposition"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryInflectablePOS, {u"noun", u"adjective", u"verb", u"adverb"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->partOfSpeechFeature = model.getFeature(GrammemeConstants::POS);
}

PtGrammarSynthesizer_PtDisplayFunction::~PtGrammarSynthesizer_PtDisplayFunction()
{

}

::std::u16string PtGrammarSynthesizer_PtDisplayFunction::guessPluralInflection(const ::std::u16string& word) const {
    int64_t wordType = 0;
    dictionary.getCombinedBinaryType(&wordType, word);
    if (wordType != 0 && ((wordType & dictionaryInflectablePOS) == 0)) {
        return word;
    }
    ::std::u16string resultString;
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"ês")) {
        return word.substr(0, word.length() - 2) + u"eses";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"r") || ::morphuntion::util::StringViewUtils::endsWith(word, u"z") || ::morphuntion::util::StringViewUtils::endsWith(word, u"s")) {
        return word + u"es";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"m")) {
        return word.substr(0, word.length() - 1) + u"ns";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"al") || ::morphuntion::util::StringViewUtils::endsWith(word, u"el") || ::morphuntion::util::StringViewUtils::endsWith(word, u"ol")|| ::morphuntion::util::StringViewUtils::endsWith(word, u"ul")) {
        return word.substr(0, word.length() - 1) + u"is";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"ão")) {
        return word.substr(0, word.length() - 2) + u"ões";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"il")) {
        return word.substr(0, word.length() - 1) + u"s";
    }
    else if (!::morphuntion::util::StringViewUtils::endsWith(word, u"x")) {
        return word + u"s";
    }
    return word;
}

::std::optional<::std::u16string> PtGrammarSynthesizer_PtDisplayFunction::inflectWord(::std::u16string_view word, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature, genderFeature}));
    const auto dismbiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));
    const auto inflectedWord = dictionaryInflector.inflect(word, constraintsVec, dismbiguationGrammemeValues);
    if (inflectedWord) {
        return *inflectedWord;
    }
    if (!enableInflectionGuess) {
        return {};
    }

    if (GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature) == GrammemeConstants::NUMBER_PLURAL()) {
        return guessPluralInflection(::std::u16string(word));
    }
    return ::std::u16string(word);
}

::std::optional<::std::vector<::std::u16string>> PtGrammarSynthesizer_PtDisplayFunction::inflect2compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    int64_t wordType[2] = { };
    ::std::u16string_view word0(words.at(0));
    ::std::u16string_view word1(words.at(1));
    dictionary.getCombinedBinaryType(&wordType[0], word0);
    dictionary.getCombinedBinaryType(&wordType[1], word1);
    //Some word is missing from the dictionary
    if (wordType[0] == 0 || wordType[1] == 0) {
        if (enableInflectionGuess) {
            return {words};
        }
    } else if ((wordType[0] & dictionaryAdjective) != 0 && (wordType[0] & dictionaryNoun) == 0 && (wordType[1] & dictionaryNoun) != 0) {
        return {words};
    } else if ((wordType[0] & dictionaryAdverb) != 0 && (wordType[1] & dictionaryAdjective) != 0) {
        const auto &inflectionResult1 = inflectWord(word1, constraints, enableInflectionGuess);
        if (inflectionResult1) {
            return {{::std::u16string(word0), *inflectionResult1}};
        }
    } else if ((wordType[0] & dictionaryNoun) != 0 && (wordType[0] & dictionaryVerb) == 0 && ((wordType[1] & dictionaryNoun) != 0 || (wordType[1] & dictionaryAdjective) != 0)) {
        const auto &inflectionResult0 = inflectWord(word0, constraints, enableInflectionGuess);
        const auto &inflectionResult1 = inflectWord(word1, constraints, enableInflectionGuess);
        if (inflectionResult0 && inflectionResult1) {
            return {{*inflectionResult0, *inflectionResult1}};
        }
    } else if (((wordType[0] & dictionaryVerb) != 0 && (wordType[1] & dictionaryNoun) != 0) || ((wordType[0] & dictionaryAdjective) != 0 && (wordType[1] & dictionaryAdjective) != 0)) {
        const auto &inflectionResult1 = inflectWord(word1, constraints, enableInflectionGuess);
        if (inflectionResult1) {
            return {{::std::u16string(word0), *inflectionResult1}};
        }
    }
    if (enableInflectionGuess) {
        return {words};
    }
    return {};
}

::std::optional<::std::vector<::std::u16string>> PtGrammarSynthesizer_PtDisplayFunction::inflect3compoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    int64_t middle = 0;
    if (dictionary.getCombinedBinaryType(&middle, words[1]) != nullptr && (middle & dictionaryAdposition) != 0) {
        int64_t wordType = 0;
        dictionary.getCombinedBinaryType(&wordType, words[0]);
        const auto& inflectionResult0 = inflectWord(words[0], constraints, enableInflectionGuess);
        if (inflectionResult0) {
            return {{*inflectionResult0, words[1], words[2]}};
        } else {
            return {words};
        }
    }
    return {words};
}

::std::optional<::std::vector<::std::u16string>> PtGrammarSynthesizer_PtDisplayFunction::inflectCompoundWord(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    int64_t numNouns = 0;
    ::std::vector<::std::u16string> inflectionResults;
    for (const auto& word : words) {
        int64_t wordType = 0;
        dictionary.getCombinedBinaryType(&wordType, word);
        if ((wordType & dictionaryNoun) != 0 && ((wordType & dictionaryAdjective) == 0)) {
            numNouns += 1;
        }
        if (numNouns >= 2) {
            inflectionResults.push_back(word);
            continue;
        }
        const auto &inflectionResult = inflectWord(word, constraints, enableInflectionGuess);
        if (inflectionResult) {
            inflectionResults.push_back(*inflectionResult);
        } else if(enableInflectionGuess){
            inflectionResults.push_back(word);
        } else {
            return {};
        }
    }
    return inflectionResults;
}

::std::optional<::std::vector<::std::u16string>> PtGrammarSynthesizer_PtDisplayFunction::inflectSignificantWords(const std::vector<::std::u16string> &words, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const {
    switch (words.size()) {
        case 0: return {{}};
        case 1: {
            const auto inflectionResult = inflectWord(words[0], constraints, enableInflectionGuess);
            if (inflectionResult) {
                return {{*inflectionResult}};
            }
            return {};
        }
        case 2:  return inflect2compoundWord(words, constraints, enableInflectionGuess);
        case 3:  return inflect3compoundWord(words, constraints, enableInflectionGuess);
        default: return inflectCompoundWord (words, constraints, enableInflectionGuess);
    }
}

::std::optional<::std::u16string> PtGrammarSynthesizer_PtDisplayFunction::inflectTokenChain(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const{
    const auto &inflectedSignificantWordsResult = inflectSignificantWords(GrammarSynthesizerUtil::getSignificantWords(tokenChain), constraints, enableInflectionGuess);
    if (!inflectedSignificantWordsResult) {
        return std::nullopt;
    }
    return GrammarSynthesizerUtil::getStringFromInflectedSignificantWords(tokenChain, *inflectedSignificantWordsResult);
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * PtGrammarSynthesizer_PtDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    auto displayValueConstraints(GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {countFeature, genderFeature})) {
        int64_t wordType = 0;
        bool needTokenizedInflection = true;
        if (dictionary.getCombinedBinaryType(&wordType, displayString) != nullptr) {
            auto inflectionResult(inflectWord(displayString, constraints, false));
            if (inflectionResult) {
                displayString = *inflectionResult;
                needTokenizedInflection = false;
            }
        }
        if (needTokenizedInflection) {
            ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(tokenizer.get())->createTokenChain(displayString));
            auto inflectionResult(inflectTokenChain(*tokenChain, constraints, enableInflectionGuess));
            if (inflectionResult) {
                displayString = *inflectionResult;
            } else if (!enableInflectionGuess) {
                return nullptr;
            }
        }
    }
    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace morphuntion::grammar::synthesis
