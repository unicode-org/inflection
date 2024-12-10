/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_CountLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/tokenizer/Token.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

EnGrammarSynthesizer_CountLookupFunction::EnGrammarSynthesizer_CountLookupFunction()
    : super(::morphuntion::util::LocaleUtils::ENGLISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}, {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB()})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::ENGLISH()))
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&prepositionProperty, {u"adposition"}));
}

EnGrammarSynthesizer_CountLookupFunction::~EnGrammarSynthesizer_CountLookupFunction()
{

}

::std::u16string EnGrammarSynthesizer_CountLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (!out.empty() || word.empty()) {
        return out;
    }
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
    const ::morphuntion::tokenizer::Token* lastWordToken = nullptr;
    for (const auto& token : *tokenChain) {
        if (dynamic_cast<const ::morphuntion::tokenizer::Token_Word*>(&token) != nullptr) {
            if (lastWordToken != nullptr && dictionary.hasAllProperties(token.getCleanValue(), prepositionProperty)) {
                // Handle "to the red light on the front porch" where "light" is the noun to check.
                out = super::determine(npc(lastWordToken)->getValue());
                if (!out.empty()) {
                    return out;
                }
                break;
            }
            lastWordToken = &token;
        }
    }
    return super::determine(npc(npc(tokenChain->getEnd())->getPrevious())->getValue());
}

} // namespace morphuntion::grammar::synthesis
