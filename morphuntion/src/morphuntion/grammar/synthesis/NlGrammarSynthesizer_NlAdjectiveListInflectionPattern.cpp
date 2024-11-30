/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveListInflectionPattern.hpp>

#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_NlAdjectiveListInflectionPattern::NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super(model)
    , nlAdjectiveInflector(model)
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::DUTCH()))
{
}

NlGrammarSynthesizer_NlAdjectiveListInflectionPattern::~NlGrammarSynthesizer_NlAdjectiveListInflectionPattern()
{

}

bool NlGrammarSynthesizer_NlAdjectiveListInflectionPattern::isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
    auto words = GrammarSynthesizerUtil::getSignificantWords(*tokenChain);
    if (words.size() < 2) {
        return false;
    }
    for (const auto& word : words) {
        if (!isAdjective(word)) {
            logUnsupportedInflection(displayString, u"contains word that is not an adjective or not in dictionary: " + word);
            return false;
        }
    }
    return isValidDeclensionConstraint(constraints);
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveListInflectionPattern::inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isSupported(displayString, constraints)) {
        return {};
    }
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, *tokenChain, *this);
}

::std::vector<::std::u16string> NlGrammarSynthesizer_NlAdjectiveListInflectionPattern::inflectSignificantTokens(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    ::std::u16string declensionStringValue(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
    NlGrammarSynthesizer::Declension targetDeclension = NlGrammarSynthesizer::getDeclension(&declensionStringValue); // Shouldn't be null, checked by isSupported

    for (auto index : indexesOfSignificant) {
        npc(tokens)->at(index) = nlAdjectiveInflector.inflectWithDeclension((*tokens)[index], targetDeclension);
    }
    return *npc(tokens);
}

} // namespace morphuntion::grammar::synthesis
