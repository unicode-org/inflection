/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_GenderLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

ItGrammarSynthesizer_GenderLookupFunction::ItGrammarSynthesizer_GenderLookupFunction()
    : super(::morphuntion::util::LocaleUtils::ITALIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::ITALIAN()))
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

ItGrammarSynthesizer_GenderLookupFunction::~ItGrammarSynthesizer_GenderLookupFunction()
{

}

::std::u16string ItGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    auto out = super::determine(word);
    if (out.empty() && !word.empty()) {
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
        for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
            if (dynamic_cast< const ::morphuntion::tokenizer::Token_Word* >(token.get()) != nullptr && dictionary.hasAllProperties(token->getCleanValue(), nounProperty)) {
                out = super::determine(token->getValue());
                break;
            }
        }
        if (out.empty()) {
            for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
                if (dynamic_cast< const ::morphuntion::tokenizer::Token_Word* >(token.get()) != nullptr) {
                    out = super::determine(token->getValue());
                    break;
                }
            }
        }
        if (out.empty()) {
            auto token = npc(tokenChain->getHead())->getNext();
            auto stringToken = npc(token)->getCleanValue();
            if (::morphuntion::util::StringViewUtils::endsWith(stringToken, u"zione") || ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"sione") || ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"gione")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"si")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"à")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"ù")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"trice")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"tudine")) {
                out = GrammemeConstants::GENDER_FEMININE();
            }
            else if (::morphuntion::util::StringViewUtils::endsWith(stringToken, u"amma") || ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"ema") || ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"ore")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"è")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"ì")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"ò")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"i")|| ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"o")) {
                out = GrammemeConstants::GENDER_MASCULINE();
            }
            else if (::morphuntion::util::StringViewUtils::endsWith(stringToken, u"a") || ::morphuntion::util::StringViewUtils::endsWith(stringToken, u"e")) {
                out = GrammemeConstants::GENDER_FEMININE();
            }
        }
    }
    return out;
}

} // namespace morphuntion::grammar::synthesis
