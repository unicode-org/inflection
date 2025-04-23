/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_CountLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

ItGrammarSynthesizer_CountLookupFunction::ItGrammarSynthesizer_CountLookupFunction()
    : super(::inflection::util::LocaleUtils::ITALIAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ITALIAN()))
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

ItGrammarSynthesizer_CountLookupFunction::~ItGrammarSynthesizer_CountLookupFunction()
{

}

::std::u16string ItGrammarSynthesizer_CountLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (out.empty() && !word.empty()) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
        for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
            if (dynamic_cast< const ::inflection::tokenizer::Token_Word* >(token.get()) != nullptr && dictionary.hasAllProperties(token->getCleanValue(), nounProperty)) {
                out = super::determine(token->getValue());
                break;
            }
        }
        if (out.empty()) {
            for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
                if (dynamic_cast< const ::inflection::tokenizer::Token_Word* >(token.get()) != nullptr) {
                    out = super::determine(token->getValue());
                    break;
                }
            }
        }
        if (out.empty()) {
            auto token = npc(tokenChain->getHead())->getNext();
            auto stringToken = npc(token)->getCleanValue();
            if (stringToken.ends_with(u"a") || stringToken.ends_with(u"o")) {
                out = GrammemeConstants::NUMBER_SINGULAR();
            }
            else if (stringToken.ends_with(u"i")) {
                out = GrammemeConstants::NUMBER_PLURAL();
            }
        }
    }
    return out;
}

} // namespace inflection::grammar::synthesis
