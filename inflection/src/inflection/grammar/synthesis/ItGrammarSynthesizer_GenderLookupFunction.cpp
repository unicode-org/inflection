/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_GenderLookupFunction.hpp>

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

ItGrammarSynthesizer_GenderLookupFunction::ItGrammarSynthesizer_GenderLookupFunction()
    : super(::inflection::util::LocaleUtils::ITALIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ITALIAN()))
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

ItGrammarSynthesizer_GenderLookupFunction::~ItGrammarSynthesizer_GenderLookupFunction()
{

}

::std::u16string ItGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
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
            if (stringToken.ends_with(u"zione") || stringToken.ends_with(u"sione") || stringToken.ends_with(u"gione")|| stringToken.ends_with(u"si")|| stringToken.ends_with(u"à")|| stringToken.ends_with(u"ù")|| stringToken.ends_with(u"trice")|| stringToken.ends_with(u"tudine")) {
                out = GrammemeConstants::GENDER_FEMININE();
            }
            else if (stringToken.ends_with(u"amma") || stringToken.ends_with(u"ema") || stringToken.ends_with(u"ore")|| stringToken.ends_with(u"è")|| stringToken.ends_with(u"ì")|| stringToken.ends_with(u"ò")|| stringToken.ends_with(u"i")|| stringToken.ends_with(u"o")) {
                out = GrammemeConstants::GENDER_MASCULINE();
            }
            else if (stringToken.ends_with(u"a") || stringToken.ends_with(u"e")) {
                out = GrammemeConstants::GENDER_FEMININE();
            }
        }
    }
    return out;
}

} // namespace inflection::grammar::synthesis
