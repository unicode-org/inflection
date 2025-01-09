/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern.hpp>

#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::DUTCH()))
    , nlAdjectiveInflector(model)
    , nlNounInflector(model)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {u"neuter"}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
}

NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::~NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern()
{

}

bool NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
    auto words = GrammarSynthesizerUtil::getSignificantWords(*tokenChain);
    if (words.empty()) {
        return false;
    }
    if (!nlNounInflector.isSupported(words.back(), constraints)) {
        logUnsupportedInflection(displayString, u"last word is not a known noun");
        return false;
    }
    int32_t wordLimit = int32_t(words.size() - 1);
    for (int32_t i = 0; i < wordLimit; i++) {
        const auto& adjWord = words[i];
        if (!isAdjective(adjWord)) {
            logUnsupportedInflection(displayString, u"one of leading words is not a known adjective: " + adjWord);
            return false;
        }
    }
    return true;
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isSupported(displayString, constraints)) {
        return {};
    }
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, *tokenChain, *this);
}

::std::vector<::std::u16string> NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto headWordIndex = indexesOfSignificant.back();
    auto headWord = (*tokens)[headWordIndex];
    npc(tokens)->at(headWordIndex) = nlNounInflector.inflect(headWord, constraints);
    if ((*tokens)[headWordIndex].empty()) {
        return {};
    }
    auto targetCountString(GrammarSynthesizerUtil::getFeatureValue(constraints, nlNounInflector.countFeature));
    auto targetCount = NlGrammarSynthesizer::getCount(&targetCountString);
    auto genderString(getGender(headWord));
    auto gender = NlGrammarSynthesizer::getGender(&genderString);
    auto targetDeclension = getAdjectiveDeclension(targetCount, gender);
    int32_t indexesOfSignificantLimit = int32_t(indexesOfSignificant.size() - 1);
    for (int32_t j = 0; j < indexesOfSignificantLimit; j++) {
        auto index = indexesOfSignificant[j];
        auto inflectedToken = nlAdjectiveInflector.inflectWithDeclension((*tokens)[index], targetDeclension);
        if (inflectedToken.empty()) {
            return {};
        }
        npc(tokens)->at(index) = inflectedToken;
    }
    return *npc(tokens);
}

NlGrammarSynthesizer::Declension NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::getAdjectiveDeclension(NlGrammarSynthesizer::Count nounCount, NlGrammarSynthesizer::Gender nounGender) const
{
    if (nounCount == NlGrammarSynthesizer::Count::undefined) {
        return NlGrammarSynthesizer::Declension::undefined;
    }
    else if (nounCount == NlGrammarSynthesizer::Count::plural) {
        return NlGrammarSynthesizer::Declension::declined;
    }
    else if (nounCount == NlGrammarSynthesizer::Count::singular) {
        if (nounGender == NlGrammarSynthesizer::Gender::undefined) {
            return NlGrammarSynthesizer::Declension::undefined;
        }
        else if (nounGender == NlGrammarSynthesizer::Gender::neuter) {
            return NlGrammarSynthesizer::Declension::undeclined;
        }
        return NlGrammarSynthesizer::Declension::declined;
    }

    return NlGrammarSynthesizer::Declension::undefined;
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern::getGender(const ::std::u16string& word) const
{
    int64_t binaryType = 0;
    if (dictionary.getCombinedBinaryType(&binaryType, word) != nullptr) {
        if ((binaryType & dictionaryGenderMask) == dictionaryFeminine) {
            return GrammemeConstants::GENDER_FEMININE();
        } else if ((binaryType & dictionaryGenderMask) == dictionaryMasculine) {
            return GrammemeConstants::GENDER_MASCULINE();
        } else if ((binaryType & dictionaryGenderMask) == dictionaryNeuter) {
            return GrammemeConstants::GENDER_NEUTER();
        }
    }
    return {};
}

} // namespace inflection::grammar::synthesis
