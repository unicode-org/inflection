/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer_GenericLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

PtGrammarSynthesizer_GenericLookupFunction::PtGrammarSynthesizer_GenericLookupFunction(const ::std::vector<::std::u16string>& properties)
    : super(::morphuntion::util::LocaleUtils::PORTUGUESE(), properties, {GrammemeConstants::POS_NOUN()})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::PORTUGUESE()))
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
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(tokenizer.get())->createTokenChain(word));
    for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
        if (dynamic_cast< const ::morphuntion::tokenizer::Token_Word* >(token.get()) != nullptr) {
            return super::determine(token->getValue());
        }
    }
    return {};
}


} // namespace morphuntion::grammar::synthesis
