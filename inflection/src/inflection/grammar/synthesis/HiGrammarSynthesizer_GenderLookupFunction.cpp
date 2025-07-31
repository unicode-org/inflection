/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_GenderLookupFunction.hpp>

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

HiGrammarSynthesizer_GenderLookupFunction::HiGrammarSynthesizer_GenderLookupFunction()
    : super(::inflection::util::LocaleUtils::HINDI(), {u"masculine", u"feminine"})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::HINDI()))
    , dictionary(getDictionary())
    , feminineSuffixes({
        u"ी"
       , u"े" // honorific
    })
    , masculineSuffixes({
        u"ा"
        , u"े" // honorific
    })
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

HiGrammarSynthesizer_GenderLookupFunction::~HiGrammarSynthesizer_GenderLookupFunction()
{
}

::std::u16string HiGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    if (word.empty()) {
        return {};
    }
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
            const auto& stringToken = npc(token)->getCleanValue();
            for (const auto& suffix : masculineSuffixes) {
                if (stringToken.ends_with(suffix)) {
                    out = GrammemeConstants::GENDER_MASCULINE();
                    break;
                }
            }
            if (out.empty()) {
                for (const auto& suffix : feminineSuffixes) {
                    if (stringToken.ends_with(suffix)) {
                        out = GrammemeConstants::GENDER_FEMININE();
                        break;
                    }
                }
            }
        }
    }
    if (out.empty()) {
        out = GrammemeConstants::GENDER_MASCULINE();
    }
    return out;
}

} // namespace inflection::grammar::synthesis
