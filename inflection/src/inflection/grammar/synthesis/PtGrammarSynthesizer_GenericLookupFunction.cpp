/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/PtGrammarSynthesizer_GenericLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

PtGrammarSynthesizer_GenericLookupFunction::PtGrammarSynthesizer_GenericLookupFunction(const ::std::vector<::std::u16string>& properties)
    : super(::inflection::util::LocaleUtils::PORTUGUESE(), properties, {GrammemeConstants::POS_NOUN()})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::PORTUGUESE()))
{
}

PtGrammarSynthesizer_GenericLookupFunction::~PtGrammarSynthesizer_GenericLookupFunction()
{
}

::std::u16string PtGrammarSynthesizer_GenericLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty() || word.empty()) {
        return out;
    }
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer.get())->createTokenChain(word));
    for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
        if (dynamic_cast< const ::inflection::tokenizer::Token_Word* >(token.get()) != nullptr) {
            return super::determine(token->getValue());
        }
    }
    return {};
}


} // namespace inflection::grammar::synthesis
