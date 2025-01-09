/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_EsDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>
#include <string>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

namespace inflection::grammar::synthesis {

EsGrammarSynthesizer_EsDisplayFunction::EsGrammarSynthesizer_EsDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , definiteArticleLookupFunction(model, EsGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const EsGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(EsGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, EsGrammarSynthesizer::ARTICLE_INDEFINITE, *npc(java_cast<const EsGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(EsGrammarSynthesizer::ARTICLE_INDEFINITE))))))
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, EsGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, EsGrammarSynthesizer::ARTICLE_INDEFINITE)
    , determinerAdpositionDetectionFunction(model,
        {//EsGrammarSynthesizer::ARTICLE_DEFINITE, EsGrammarSynthesizer::ARTICLE_INDEFINITE,
        EsGrammarSynthesizer::WITH_ARTICLE_DE_PREPOSITION, EsGrammarSynthesizer::WITH_ARTICLE_A_PREPOSITION}
    )
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::SPANISH())))
    , dictionaryInflector(::inflection::util::LocaleUtils::SPANISH(), {
                {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
                {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
                {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()},
                {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    }, {}, true)
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SPANISH()))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {u"singular"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPreposition, {u"adposition"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->partOfSpeechFeature = model.getFeature(GrammemeConstants::POS);
}

EsGrammarSynthesizer_EsDisplayFunction::~EsGrammarSynthesizer_EsDisplayFunction()
{

}

::std::u16string EsGrammarSynthesizer_EsDisplayFunction::guessGenderedInflection(const ::std::u16string& word, ::std::u16string_view gender) const
{
    if (gender == GrammemeConstants::GENDER_MASCULINE() && !(::inflection::util::StringViewUtils::endsWith(word, u"o") || ::inflection::util::StringViewUtils::endsWith(word, u"os"))) {
        if (::inflection::util::StringViewUtils::endsWith(word, u"a")) {
            return word.substr(0, word.length() - 1) + u"o";
        }
        else if (::inflection::util::StringViewUtils::endsWith(word, u"as")) {
            return word.substr(0, word.length() - 2) + u"os";
        }
    }
    if (gender == GrammemeConstants::GENDER_FEMININE() && !(::inflection::util::StringViewUtils::endsWith(word, u"a") || ::inflection::util::StringViewUtils::endsWith(word, u"as"))) {
        if (::inflection::util::StringViewUtils::endsWith(word, u"o")) {
            return word.substr(0, word.length() - 1) + u"a";
        }
        else if (::inflection::util::StringViewUtils::endsWith(word, u"os")) {
            return word.substr(0, word.length() - 2) + u"as";
        }
    }
    return word;
}

::std::u16string EsGrammarSynthesizer_EsDisplayFunction::guessPluralInflection(const ::std::u16string& word) const
{
    if (::inflection::util::StringViewUtils::endsWith(word, u"s") || ::inflection::util::StringViewUtils::endsWith(word, u"x")) {
        return word;
    }
    else if (::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::SPANISH(), word)) {
        return word + u"s";
    }
    else if (::inflection::util::StringViewUtils::endsWith(word, u"z")) {
        return word.substr(0, word.length() - 1) + u"ces";
    }
    else if (::inflection::util::StringViewUtils::endsWith(word, u"ión")) {
        return word.substr(0, word.length() - 2) + u"ones";
    }
    else if (::inflection::util::StringViewUtils::endsWith(word, u"c")) {
        return word.substr(0, word.length() - 1) + u"ques";
    }
    else if (::inflection::util::StringViewUtils::endsWith(word, u"g")) {
        return word.substr(0, word.length() - 1) + u"gues";
    }
    return word + u"es";
}

::std::optional<::std::u16string> EsGrammarSynthesizer_EsDisplayFunction::inflectWord(std::u16string_view displayString, int64_t wordType, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature, genderFeature}));
    const auto dismbiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));
    const auto inflectionResult = dictionaryInflector.inflect(displayString, constraintsVec, dismbiguationGrammemeValues);
    if (inflectionResult) {
        return *inflectionResult;
    }

    if (!enableInflectionGuess) {
        return {};
    }

    // Well, dictionary was not able to inflect it. So let's make a guess.
    ::std::u16string inflectedWord(displayString);
    if (wordType != 0) {
        return inflectedWord;
    }
    if (GrammarSynthesizerUtil::hasFeature(constraints, genderFeature)) {
        // We requested a gender, but it's not a known word. Make a guess.
        inflectedWord = guessGenderedInflection(inflectedWord, GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
    }
    if (GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature) == GrammemeConstants::NUMBER_PLURAL()) {
        inflectedWord = guessPluralInflection(inflectedWord);
    }
    return inflectedWord;
}

::std::optional<::std::u16string> EsGrammarSynthesizer_EsDisplayFunction::inflectCompoundWord(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    auto containsPrep = false;
    ::std::u16string inflectedString;
    for (const auto& tNext : tokenChain) {
        const auto& word = tNext.getValue();
        if (!GrammarSynthesizerUtil::checkSignificantTokenForInflection(tNext) || containsPrep) {
            inflectedString += word;
            continue;
        }
        int64_t wordType = 0;
        dictionary.getCombinedBinaryType(&wordType, word);
        if ((wordType & dictionaryPreposition) != 0) {
            containsPrep = true;
            inflectedString += word;
            continue;
        }
        const auto inflectionResult = inflectWord(word, wordType, constraints, enableInflectionGuess);
        auto inflectionValue = word;
        if (inflectionResult) {
            inflectionValue = *inflectionResult;
        } else if(!enableInflectionGuess) {
            return {};
        }
        inflectedString += inflectionValue;
    }
    return inflectedString;
}

::inflection::dialog::SemanticFeatureModel_DisplayValue * EsGrammarSynthesizer_EsDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

    const auto &articleData = determinerAdpositionDetectionFunction.detectAndStripArticlePrefix(displayString);

    ::std::optional<::std::u16string> inflectionResult;
    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {countFeature, genderFeature})) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        if (tokenChain->getWordCount() == 1) {
            int64_t wordType = 0;
            dictionary.getCombinedBinaryType(&wordType, displayString);
            inflectionResult = inflectWord(displayString, wordType, constraints, enableInflectionGuess);
        }
        else {
            inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), constraints, enableInflectionGuess);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess){
            return nullptr;
        }
    }

    GrammarSynthesizerUtil::inflectAndAppendArticlePrefix(displayString, displayValueConstraints, articleData.first, articleData.second);
    return definitenessDisplayFunction.addDefiniteness(new ::inflection::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace inflection::grammar::synthesis
