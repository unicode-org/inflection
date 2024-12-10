/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer_GenderLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

HiGrammarSynthesizer_GenderLookupFunction::HiGrammarSynthesizer_GenderLookupFunction()
    : super(::morphuntion::util::LocaleUtils::HINDI(), {u"masculine", u"feminine"})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::HINDI()))
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

HiGrammarSynthesizer_GenderLookupFunction::~HiGrammarSynthesizer_GenderLookupFunction()
{

}

static const ::std::set<::std::u16string_view>& FEMININE_SUFFIXES()
{
    static auto FEMININE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ी"
       , u"े" //honourific
    });
    return *npc(FEMININE_SUFFIXES_);
}

static const ::std::set<::std::u16string_view>& MASCULINE_SUFFIXES()
{
    static auto MASCULINE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ा"
        , u"े" //honourific
    });
    return *npc(MASCULINE_SUFFIXES_);
}

::std::u16string HiGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    if (word.empty()) {
        return {};
    }
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
            const auto& stringToken = npc(token)->getCleanValue();
            for (const auto& suffix : MASCULINE_SUFFIXES()) {
                if (::morphuntion::util::StringViewUtils::endsWith(stringToken, suffix)) {
                    out = GrammemeConstants::GENDER_MASCULINE();
                    break;
                }
            }
            if (out.empty()) {
                for (const auto& suffix : FEMININE_SUFFIXES()) {
                    if (::morphuntion::util::StringViewUtils::endsWith(stringToken, suffix)) {
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

} // namespace morphuntion::grammar::synthesis
