/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/GrammemeLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar {

GrammemeLookupFunction::GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech)
    : super(locale, tags, disambiguationPartsOfSpeech)
    , relevantGrammemes(disambiguationPartsOfSpeech.empty() ? 0 : getDictionary().getBinaryProperties({inflection::grammar::synthesis::GrammemeConstants::POS_NOUN}))
    , firstWordDeterminesValue(false)
{
}

GrammemeLookupFunction::GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech, bool firstWordDeterminesValue)
    : super(locale, tags, disambiguationPartsOfSpeech)
    , relevantGrammemes(disambiguationPartsOfSpeech.empty() ? 0 : getDictionary().getBinaryProperties({inflection::grammar::synthesis::GrammemeConstants::POS_NOUN}))
    , firstWordDeterminesValue(firstWordDeterminesValue)
{
}

GrammemeLookupFunction::GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::u16string& defaultValue, bool firstWordDeterminesValue)
    : super(locale, tags)
    , relevantGrammemes(getDictionary().getBinaryProperties({inflection::grammar::synthesis::GrammemeConstants::POS_NOUN}))
    , defaultValue(defaultValue)
    , firstWordDeterminesValue(firstWordDeterminesValue)
{
}

GrammemeLookupFunction::GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech, const ::std::u16string& defaultValue, bool firstWordDeterminesValue, SuffixFunction suffixFunction)
    : super(locale, tags, disambiguationPartsOfSpeech)
    , relevantGrammemes(disambiguationPartsOfSpeech.empty() ? 0 : getDictionary().getBinaryProperties({inflection::grammar::synthesis::GrammemeConstants::POS_NOUN}))
    , defaultValue(defaultValue)
    , firstWordDeterminesValue(firstWordDeterminesValue)
    , suffixFunction(std::move(suffixFunction))
{
}

GrammemeLookupFunction::~GrammemeLookupFunction() = default;

::std::u16string GrammemeLookupFunction::determine(const ::std::u16string& word) const
{
    if (word.empty()) {
        return {};
    }
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(getTokenizer().createTokenChain(word)));
    if (firstWordDeterminesValue) {
        for (const auto& token : *tokenChain) {
            if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(&token) != nullptr
                && (relevantGrammemes == 0 || getDictionary().hasAllProperties(token.getCleanValue(), relevantGrammemes)))
            {
                out = super::determine(token.getValue());
                break;
            }
        }
        if (out.empty()) {
            for (const auto& token : *tokenChain) {
                if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(&token) != nullptr) {
                    out = super::determine(token.getValue());
                    break;
                }
            }
        }
    } else {
        for (const auto& token : *tokenChain) {
            if (dynamic_cast<const ::inflection::tokenizer::Token_Word*>(&token) != nullptr
                && (relevantGrammemes == 0 || getDictionary().hasAllProperties(token.getCleanValue(), relevantGrammemes)))
            {
                auto tmp = super::determine(npc(&token)->getValue());
                if (!tmp.empty()) {
                    out = tmp;
                }
            }
        }
        if (out.empty()) {
            out = super::determine(npc(npc(tokenChain->getTail())->getPrevious())->getValue());
        }
    }
    if (out.empty() && suffixFunction) {
        auto headNext = npc(tokenChain->getHead())->getNext();
        if (headNext != nullptr) {
            out = suffixFunction(npc(headNext)->getCleanValue());
        }
    }
    if (out.empty()) {
        return defaultValue;
    }
    return out;
}

} // namespace inflection::grammar
