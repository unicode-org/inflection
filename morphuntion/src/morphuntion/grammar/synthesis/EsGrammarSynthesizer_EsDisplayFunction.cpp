/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_EsDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>
#include <string>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::morphuntion::exception::ClassCastException();
    return t;
}

namespace morphuntion::grammar::synthesis {

EsGrammarSynthesizer_EsDisplayFunction::EsGrammarSynthesizer_EsDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , definiteArticleLookupFunction(model, EsGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const EsGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(EsGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, EsGrammarSynthesizer::ARTICLE_INDEFINITE, *npc(java_cast<const EsGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(EsGrammarSynthesizer::ARTICLE_INDEFINITE))))))
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, EsGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, EsGrammarSynthesizer::ARTICLE_INDEFINITE)
    , determinerAdpositionDetectionFunction(model,
        {//EsGrammarSynthesizer::ARTICLE_DEFINITE, EsGrammarSynthesizer::ARTICLE_INDEFINITE,
        EsGrammarSynthesizer::WITH_ARTICLE_DE_PREPOSITION, EsGrammarSynthesizer::WITH_ARTICLE_A_PREPOSITION}
    )
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::SPANISH())))
    , dictionaryInflector(::morphuntion::util::LocaleUtils::SPANISH(), {
                {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
                {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
                {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()},
                {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    }, {}, true)
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::SPANISH()))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {u"singular"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPreposition, {u"adposition"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->partOfSpeechFeature = model.getFeature(GrammemeConstants::POS);
}

EsGrammarSynthesizer_EsDisplayFunction::~EsGrammarSynthesizer_EsDisplayFunction()
{

}

::std::u16string EsGrammarSynthesizer_EsDisplayFunction::guessGenderedInflection(const ::std::u16string& word, ::std::u16string_view gender) const
{
    if (gender == GrammemeConstants::GENDER_MASCULINE() && !(::morphuntion::util::StringViewUtils::endsWith(word, u"o") || ::morphuntion::util::StringViewUtils::endsWith(word, u"os"))) {
        if (::morphuntion::util::StringViewUtils::endsWith(word, u"a")) {
            return word.substr(0, word.length() - 1) + u"o";
        }
        else if (::morphuntion::util::StringViewUtils::endsWith(word, u"as")) {
            return word.substr(0, word.length() - 2) + u"os";
        }
    }
    if (gender == GrammemeConstants::GENDER_FEMININE() && !(::morphuntion::util::StringViewUtils::endsWith(word, u"a") || ::morphuntion::util::StringViewUtils::endsWith(word, u"as"))) {
        if (::morphuntion::util::StringViewUtils::endsWith(word, u"o")) {
            return word.substr(0, word.length() - 1) + u"a";
        }
        else if (::morphuntion::util::StringViewUtils::endsWith(word, u"os")) {
            return word.substr(0, word.length() - 2) + u"as";
        }
    }
    return word;
}

::std::u16string EsGrammarSynthesizer_EsDisplayFunction::guessPluralInflection(const ::std::u16string& word) const
{
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"s") || ::morphuntion::util::StringViewUtils::endsWith(word, u"x")) {
        return word;
    }
    else if (::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::SPANISH(), word)) {
        return word + u"s";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"z")) {
        return word.substr(0, word.length() - 1) + u"ces";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"ión")) {
        return word.substr(0, word.length() - 2) + u"ones";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"c")) {
        return word.substr(0, word.length() - 1) + u"ques";
    }
    else if (::morphuntion::util::StringViewUtils::endsWith(word, u"g")) {
        return word.substr(0, word.length() - 1) + u"gues";
    }
    return word + u"es";
}

::std::optional<::std::u16string> EsGrammarSynthesizer_EsDisplayFunction::inflectWord(std::u16string_view displayString, int64_t wordType, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

::std::optional<::std::u16string> EsGrammarSynthesizer_EsDisplayFunction::inflectCompoundWord(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * EsGrammarSynthesizer_EsDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
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
    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace morphuntion::grammar::synthesis
