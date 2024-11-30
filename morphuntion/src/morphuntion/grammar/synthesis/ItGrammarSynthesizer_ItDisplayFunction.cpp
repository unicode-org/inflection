/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_ItDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
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

ItGrammarSynthesizer_ItDisplayFunction::ItGrammarSynthesizer_ItDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , partOfSpeechFeature(model.getFeature(GrammemeConstants::POS))
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::ITALIAN())))
    , dictionaryInflector(::morphuntion::util::LocaleUtils::ITALIAN(), {
                {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
                {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
                {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()},
                {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()}
        }, {}, true)
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::ITALIAN()))
    , definiteArticleLookupFunction(model, ItGrammarSynthesizer::ARTICLE_DEFINITE, *npc(java_cast<const ItGrammarSynthesizer_DefiniteArticleLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(ItGrammarSynthesizer::ARTICLE_DEFINITE))))))
    , indefiniteArticleLookupFunction(model, ItGrammarSynthesizer::ARTICLE_INDEFINITE)
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, ItGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, ItGrammarSynthesizer::ARTICLE_INDEFINITE)
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPreposition, {u"adposition"}));
}

ItGrammarSynthesizer_ItDisplayFunction::~ItGrammarSynthesizer_ItDisplayFunction()
{
}

::std::optional<::std::u16string> ItGrammarSynthesizer_ItDisplayFunction::inflectWord(::std::u16string_view word, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const {
    ::std::vector<::std::u16string> constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature, genderFeature}));
    ::std::vector<::std::u16string> disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));
    const auto inflectedWord(dictionaryInflector.inflect(word, constraintsVec, disambiguationGrammemeValues));
    if (inflectedWord) {
        return *inflectedWord;
    } else if (enableInflectionGuess) {
        return ::std::u16string(word);
    }
    return  {};
}

::std::optional<::std::u16string> ItGrammarSynthesizer_ItDisplayFunction::inflectCompoundWord(const morphuntion::tokenizer::TokenChain &tokenChain, const std::map<dialog::SemanticFeature, std::u16string> &constraints, bool enableInflectionGuess) const {
    auto containsPrep = false;
    ::std::u16string inflectedString;
    inflectedString.reserve(npc(tokenChain.getEnd())->getEndChar());
    for (const auto& tNext : tokenChain) {
        const auto& word = tNext.getValue();
        if (!tNext.isSignificant() || word == u"-" || containsPrep) {
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
        const auto inflectionResult = inflectWord(word, constraints, enableInflectionGuess);
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

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * ItGrammarSynthesizer_ItDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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
        ::std::optional<::std::u16string> inflectionResult;
        if (tokenChain->getWordCount() == 1) {
            inflectionResult = inflectWord(displayString, constraints, enableInflectionGuess);
        } else {
            inflectionResult = inflectCompoundWord(*npc(tokenChain.get()), constraints, enableInflectionGuess);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        } else if (!enableInflectionGuess){
            return nullptr;
        }
    }
    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

} // namespace morphuntion::grammar::synthesis
