/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer_CountLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

HiGrammarSynthesizer_CountLookupFunction::HiGrammarSynthesizer_CountLookupFunction()
    : super(::morphuntion::util::LocaleUtils::HINDI(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB()})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::HINDI()))
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));

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
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
    for (const auto& token : *tokenChain) {
        if (dynamic_cast<const ::morphuntion::tokenizer::Token_Word*>(&token) != nullptr) {
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

} // namespace morphuntion::grammar::synthesis
