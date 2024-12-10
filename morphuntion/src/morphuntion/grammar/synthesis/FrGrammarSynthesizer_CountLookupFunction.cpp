/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>

#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <string>
#include <optional>
#include <string_view>

namespace morphuntion::grammar::synthesis {

FrGrammarSynthesizer_CountLookupFunction::FrGrammarSynthesizer_CountLookupFunction()
    : super(::morphuntion::util::LocaleUtils::FRENCH(), {u"singular", u"plural"}, {u"noun", u"adjective"})
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::FRENCH()))
    , dictionary(getDictionary())
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::FRENCH()))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&properNounProperty, {u"proper-noun"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&appleProductProperty, {u"appleproduct"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&pluralizeInvProperty, {u"plural", u"singular"}));
}

FrGrammarSynthesizer_CountLookupFunction::~FrGrammarSynthesizer_CountLookupFunction()
{

}

bool FrGrammarSynthesizer_CountLookupFunction::checkProperNoun(::std::u16string_view word) const
{
    return dictionary.hasAllProperties(word, properNounProperty);
}

bool FrGrammarSynthesizer_CountLookupFunction::checkInvariantNouns(::std::u16string_view word) const
{
    if (!dictionary.hasAllProperties(word, nounProperty)) {
        return false;
    }
    const auto inflectionPatterns = dictionary.getPropertyValues(word, u"inflection");
    if (inflectionPatterns.size() <= 1) {
        return dictionary.hasAllProperties(word, pluralizeInvProperty);
    }
    return false;
}

bool FrGrammarSynthesizer_CountLookupFunction::checkAppleProduct(::std::u16string_view word) const {
    return dictionary.hasAllProperties(word, appleProductProperty);
}

::std::optional<::std::u16string> FrGrammarSynthesizer_CountLookupFunction::determineWord(const std::u16string &word) const {
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }
    if (checkAppleProduct(word)) {
        return GrammemeConstants::NUMBER_SINGULAR();
    }
    if (checkProperNoun(word)) {
        return {{}};
    }
    if (checkInvariantNouns(word)) {
        return GrammemeConstants::NUMBER_SINGULAR();
    }
    return {};
}

::std::u16string FrGrammarSynthesizer_CountLookupFunction::determine(const ::std::u16string& word) const
{
    auto result = determineWord(word);
    if (result) {
        return *result;
    }
    auto firstWord(word);
    if (!firstWord.empty()) {
        const ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(firstWord)));
        firstWord = npc(npc(tokenChain->getHead())->getNext())->getValue();
        if (firstWord.length() != word.length()) {
            result = determineWord(firstWord);
            if (result) {
                return *result;
            }
        }
    }

    if (dictionary.hasAllProperties(firstWord, nounProperty)) {
        if (::morphuntion::util::StringViewUtils::endsWith(firstWord, u"s") || ::morphuntion::util::StringViewUtils::endsWith(firstWord, u"x")) {
            return GrammemeConstants::NUMBER_PLURAL();
        } else {
            return GrammemeConstants::NUMBER_SINGULAR();
        }
    }
    return {};
}

} // namespace morphuntion::grammar::synthesis
