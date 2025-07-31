/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_FrDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

namespace inflection::grammar::synthesis {

FrGrammarSynthesizer_FrDisplayFunction::FrGrammarSynthesizer_FrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::FRENCH())))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , definiteArticleLookupFunction(model, false, FrGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const FrGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(FrGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, true, FrGrammarSynthesizer::ARTICLE_INDEFINITE, *npc(java_cast<const FrGrammarSynthesizer_ArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(FrGrammarSynthesizer::ARTICLE_INDEFINITE))))))
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, FrGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, FrGrammarSynthesizer::ARTICLE_INDEFINITE)
    , WORDS_PREVENTING_INFLECTION({u"de", u"d", u"du", u"des", u"en", u"à", u"par"})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRENCH()))
    , dictionaryInflector(::inflection::util::LocaleUtils::FRENCH(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
    }, {}, true)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounOrAdjectiveProperties, {u"noun", u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&articlePronounProperNounPrepositionProperties, {u"article", u"pronoun", u"proper-noun", u"preposition", u"adposition"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&adverbProperty, {u"adverb"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&verbProperty, {u"verb"}));
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
            return word.ends_with(u"é") || word.ends_with(u"ée");
        }
    }
    return true;
}

::std::u16string FrGrammarSynthesizer_FrDisplayFunction::guessPluralInflection(const ::std::u16string& word) const
{
    if (!canBeInflectedToPlural(word)) {
        return word;
    }
    if (word.ends_with(u"s") || word.ends_with(u"x") || word.ends_with(u"z")) {
        return word;
    }
    if (word.ends_with(u"eau") || word.ends_with(u"eu")) {
        return word + u'x';
    }
    if (word.ends_with(u"al")) {
        return word.substr(0, word.length() - 2) + u"aux";
    }
    if (word.ends_with(u"ail")) {
        return word.substr(0, word.length() - 3) + u"aux";
    }
    return word + u's';
}

::std::optional<::std::u16string> FrGrammarSynthesizer_FrDisplayFunction::inflectWord(::std::u16string_view word, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&numberFeature, &genderFeature}));
    const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
    auto inflectionResult = dictionaryInflector.inflect(word, wordGrammemes, constraintsVec, disambiguationGrammemeValues);
    if (inflectionResult) {
        return *inflectionResult;
    }
    if (!enableInflectionGuess) {
        return {};
    }

    // Well, dictionary was not able to inflect it. So let's make a guess.
    ::std::u16string inflectedWord(word);
    if (GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature) == GrammemeConstants::NUMBER_PLURAL()) {
        inflectedWord = guessPluralInflection(::std::u16string(word));
    }
    return inflectedWord;
}

::std::optional<::std::u16string> FrGrammarSynthesizer_FrDisplayFunction::inflectCompoundWord(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    ::std::u16string inflectedString;
    auto preventNextWordFromInflecting = false;
    for (const auto& token : tokenChain) {
        const auto& word = token.getValue();
        if (!GrammarSynthesizerUtil::checkSignificantTokenForInflection(token)) {
            inflectedString += word;
            continue;
        }
        if (WORDS_PREVENTING_INFLECTION.contains(word)) {
            preventNextWordFromInflecting = true;
            inflectedString += word;
            continue;
        }
        if (preventNextWordFromInflecting) {
            preventNextWordFromInflecting = false;
            inflectedString += word;
        } else {
            int64_t wordGrammemes = 0;
            dictionary.getCombinedBinaryType(&wordGrammemes, word);

            const auto &inflectionResult = inflectWord(word, wordGrammemes, constraints, enableInflectionGuess);
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

::inflection::tokenizer::TokenChain&
FrGrammarSynthesizer_FrDisplayFunction::tokenize(::std::unique_ptr<::inflection::tokenizer::TokenChain>& tokenChain, const std::u16string& string) const
{
    tokenChain.reset(npc(npc(tokenizer.get())->createTokenChain(string)));
    return *tokenChain;
}

::inflection::dialog::DisplayValue * FrGrammarSynthesizer_FrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

    if (GrammarSynthesizerUtil::hasAnyFeatures(constraints, {&numberFeature, &genderFeature})) {
        ::std::optional<::std::u16string> inflectionResult;
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain;
        int64_t wordGrammemes = 0;
        if (dictionary.getCombinedBinaryType(&wordGrammemes, displayString) != nullptr
            || tokenize(tokenChain, displayString).getWordCount() == 1)
        {
            // Either a known word, or a word to guess.
            inflectionResult = inflectWord(displayString, wordGrammemes, constraints, enableInflectionGuess);
        }
        else {
            inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), constraints, enableInflectionGuess);
        }

        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess) {
            return nullptr;
        }
    }
    return definitenessDisplayFunction.addDefiniteness(new ::inflection::dialog::DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace inflection::grammar::synthesis
