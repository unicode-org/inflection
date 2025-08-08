/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_GenderLookupFunction.hpp>

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

static bool ends_with(const std::u16string& str, const std::u16string_view& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

MlGrammarSynthesizer_GenderLookupFunction::MlGrammarSynthesizer_GenderLookupFunction()
    : super(::inflection::util::LocaleUtils::MALAYALAM(),
            {GrammemeConstants::GENDER_MASCULINE(),
             GrammemeConstants::GENDER_FEMININE(),
             GrammemeConstants::GENDER_NEUTER()},
            {GrammemeConstants::POS_NOUN(),
             GrammemeConstants::POS_PRONOUN()})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::MALAYALAM()))
    , dictionary(getDictionary())
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
}

MlGrammarSynthesizer_GenderLookupFunction::~MlGrammarSynthesizer_GenderLookupFunction()
{

}

static const ::std::set<::std::u16string_view>& FEMININE_SUFFIXES()
{
    static auto FEMININE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ി"   // e.g. പെൺ (pen) endings
       , u"ാളി"  // common feminine suffix in Malayalam nouns
    });
    return *npc(FEMININE_SUFFIXES_);
}

static const ::std::set<::std::u16string_view>& MASCULINE_SUFFIXES()
{
    static auto MASCULINE_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ൻ"   // e.g. ആൾ (person) endings
        , u"ർ"  // common masculine suffix in Malayalam nouns
    });
    return *npc(MASCULINE_SUFFIXES_);
}

static const ::std::set<::std::u16string_view>& NEUTER_SUFFIXES()
{
    static auto NEUTER_SUFFIXES_ = new ::std::set<::std::u16string_view>({
        u"ത്",
        u"ം",
        u"യം"
    });
    return *npc(NEUTER_SUFFIXES_);
}

::std::u16string MlGrammarSynthesizer_GenderLookupFunction::determine(const ::std::u16string& word) const
{
    if (word.empty()) {
        return {};
    }

    auto gender = super::determine(word);
    if (gender.empty()) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(word)));
        
        // First try dictionary lookup on noun tokens
        for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
            if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(token.get()) != nullptr &&
                dictionary.hasAllProperties(token->getCleanValue(), nounProperty)) {
                gender = super::determine(token->getValue());
                if (!gender.empty()) break;
            }
        }

        // If still empty, try any word token
        if (gender.empty()) {
            for (auto token = tokenChain->begin(); token != tokenChain->end(); ++token) {
                if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(token.get()) != nullptr) {
                    gender = super::determine(token->getValue());
                    if (!gender.empty()) break;
                }
            }
        }

        // If still empty, fallback to suffix heuristics on the second token in chain
        if (gender.empty()) {
            auto head = tokenChain->getHead();
            if (head != nullptr) {
                auto token = npc(head)->getNext();
                if (token != nullptr) {
                    const auto& stringToken = npc(token)->getCleanValue();

                    for (const auto& suffix : MASCULINE_SUFFIXES()) {
                        if (ends_with(stringToken, suffix)) {
                            gender = GrammemeConstants::GENDER_MASCULINE();
                            break;
                        }
                    }

                    if (gender.empty()) {
                        for (const auto& suffix : FEMININE_SUFFIXES()) {
                            if (ends_with(stringToken, suffix)) {
                                gender = GrammemeConstants::GENDER_FEMININE();
                                break;
                            }
                        }
                    }

                    if (gender.empty()) {
                        for (const auto& suffix : NEUTER_SUFFIXES()) {
                            if (ends_with(stringToken, suffix)) {
                                gender = GrammemeConstants::GENDER_NEUTER();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (gender.empty()) {
        // Default to masculine if no gender is detected
        gender = GrammemeConstants::GENDER_MASCULINE();
    }
    return gender;
}

} // namespace inflection::grammar::synthesis

