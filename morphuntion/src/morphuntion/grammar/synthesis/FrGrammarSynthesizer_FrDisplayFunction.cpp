/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_FrDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::morphuntion::exception::ClassCastException();
    return t;
}

namespace morphuntion::grammar::synthesis {

FrGrammarSynthesizer_FrDisplayFunction::FrGrammarSynthesizer_FrDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::FRENCH())))
    , definiteArticleLookupFunction(model, false, FrGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const FrGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(FrGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, true, FrGrammarSynthesizer::ARTICLE_INDEFINITE, *npc(java_cast<const FrGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(FrGrammarSynthesizer::ARTICLE_INDEFINITE))))))
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, FrGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, FrGrammarSynthesizer::ARTICLE_INDEFINITE)
    , WORDS_PREVENTING_INFLECTION({u"de", u"d", u"du", u"des", u"en", u"à", u"par"})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::FRENCH()))
    , dictionaryInflector(::morphuntion::util::LocaleUtils::FRENCH(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    }, {}, true)
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounOrAdjectiveProperties, {u"noun", u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&articlePronounProperNounPrepositionProperties, {u"article", u"pronoun", u"proper-noun", u"preposition", u"adposition"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&adverbProperty, {u"adverb"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&verbProperty, {u"verb"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->partOfSpeechFeature = model.getFeature(GrammemeConstants::POS);
}

FrGrammarSynthesizer_FrDisplayFunction::~FrGrammarSynthesizer_FrDisplayFunction()
{

}

bool FrGrammarSynthesizer_FrDisplayFunction::canBeInflectedToPlural(::std::u16string_view word) const
{
    if (word.length() <= 2) {
        return false;
    }
    int64_t wordProperties = 0;
    if (dictionary.getCombinedBinaryType(&wordProperties, word) != nullptr) {
        if ((wordProperties & articlePronounProperNounPrepositionProperties) != 0) {
            return false;
        }
        if ((wordProperties & nounOrAdjectiveProperties) != 0) {
            return true;
        }
        if ((wordProperties & adverbProperty) != 0) {
            return false;
        }
        if ((wordProperties & verbProperty) != 0) {
            return ::morphuntion::util::StringViewUtils::endsWith(word, u"é") || ::morphuntion::util::StringViewUtils::endsWith(word, u"ée");
        }
    }
    return true;
}

::std::u16string FrGrammarSynthesizer_FrDisplayFunction::guessPluralInflection(const ::std::u16string& word) const
{
    if (!canBeInflectedToPlural(word)) {
        return word;
    }
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"s") || ::morphuntion::util::StringViewUtils::endsWith(word, u"x") || ::morphuntion::util::StringViewUtils::endsWith(word, u"z")) {
        return word;
    }
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"eau") || ::morphuntion::util::StringViewUtils::endsWith(word, u"eu")) {
        return word + u'x';
    }
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"al")) {
        return word.substr(0, word.length() - 2) + u"aux";
    }
    if (::morphuntion::util::StringViewUtils::endsWith(word, u"ail")) {
        return word.substr(0, word.length() - 3) + u"aux";
    }
    return word + u's';
}

::std::optional<::std::u16string> FrGrammarSynthesizer_FrDisplayFunction::inflectWord(::std::u16string_view word, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature, genderFeature}));
    const auto dismbiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));
    auto inflectionResult = dictionaryInflector.inflect(word, constraintsVec, dismbiguationGrammemeValues);
    if (inflectionResult) {
        return *inflectionResult;
    }
    if (!enableInflectionGuess) {
        return {};
    }

    // Well, dictionary was not able to inflect it. So let's make a guess.
    ::std::u16string inflectedWord(word);
    if (GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature) == GrammemeConstants::NUMBER_PLURAL()) {
        inflectedWord = guessPluralInflection(::std::u16string(word));
    }
    return inflectedWord;
}

::std::optional<::std::u16string> FrGrammarSynthesizer_FrDisplayFunction::inflectCompoundWord(const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    ::std::u16string inflectedString;
    auto preventNextWordFromInflecting = false;
    for (const auto& token : tokenChain) {
        const auto& word = token.getValue();
        if (!GrammarSynthesizerUtil::checkSignificantTokenForInflection(token)) {
            inflectedString += word;
            continue;
        }
        if (WORDS_PREVENTING_INFLECTION.find(word) != WORDS_PREVENTING_INFLECTION.end()) {
            preventNextWordFromInflecting = true;
            inflectedString += word;
            continue;
        }
        if (preventNextWordFromInflecting) {
            preventNextWordFromInflecting = false;
            inflectedString += word;
        } else {
            const auto &inflectionResult = inflectWord(word, constraints, enableInflectionGuess);
            auto inflectionValue = word;
            if (inflectionResult) {
                inflectionValue = *inflectionResult;
            } else if (!enableInflectionGuess) {
                return {};
            }
            inflectedString += inflectionValue;
        }
    }
    return inflectedString;
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * FrGrammarSynthesizer_FrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        const auto &inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), constraints, enableInflectionGuess);
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess){
            return nullptr;
        }
    }
    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace morphuntion::grammar::synthesis
