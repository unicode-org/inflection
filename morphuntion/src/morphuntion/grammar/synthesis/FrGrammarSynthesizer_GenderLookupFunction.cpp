/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_GenderLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer.hpp>
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

FrGrammarSynthesizer_GenderLookupFunction::FrGrammarSynthesizer_GenderLookupFunction()
    : super(::morphuntion::util::LocaleUtils::FRENCH(), {u"masculine", u"feminine"})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::FRENCH()))
    , dictionary(getDictionary())
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

FrGrammarSynthesizer_GenderLookupFunction::~FrGrammarSynthesizer_GenderLookupFunction()
{

}

static const ::std::set<::std::u16string_view>& FEMININE_SUFFIXES()
{
    static auto FEMININE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"e"
        , u"es"
        , u"n"
        , u"ns"
        , u"é"
        , u"a"
        , u"és"
    });
    return *npc(FEMININE_SUFFIXES_);
}

static const ::std::set<::std::u16string_view>& MASCULINE_SUFFIXES()
{
    static auto MASCULINE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ge"
        , u"ges"
        , u"me"
        , u"mes"
        , u"in"
        , u"ins"
        , u"en"
        , u"an"
        , u"ens"
        , u"tre"
        , u"tres"
        , u"ire"
        , u"ires"
        , u"ans"
        , u"ste"
        , u"stes"
        , u"lon"
        , u"lons"
        , u"ton"
    });
    return *npc(MASCULINE_SUFFIXES_);
}

::std::u16string FrGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
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
