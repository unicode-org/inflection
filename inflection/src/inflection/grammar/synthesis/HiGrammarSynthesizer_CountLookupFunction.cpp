/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_CountLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

HiGrammarSynthesizer_CountLookupFunction::HiGrammarSynthesizer_CountLookupFunction()
    : super(::inflection::util::LocaleUtils::HINDI(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB()})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::HINDI()))
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));

}

HiGrammarSynthesizer_CountLookupFunction::~HiGrammarSynthesizer_CountLookupFunction()
{

}

::std::u16string HiGrammarSynthesizer_CountLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty() || word.empty()) {
        return out;
    }
    ::std::u16string returnValue = u"";
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
    for (const auto& token : *tokenChain) {
        if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(&token) != nullptr) {
            if (dictionary.hasAllProperties(token.getCleanValue(), nounProperty)) {
                out = super::determine(npc(&token)->getValue());
                if (!out.empty()) {
                    returnValue = out;
                }
            }
        }
    }
    if (!returnValue.empty()) {
        return returnValue;
    }
    return super::determine(npc(npc(tokenChain->getEnd())->getPrevious())->getValue());
}

} // namespace inflection::grammar::synthesis
