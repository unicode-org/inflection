/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>

#include <inflection/dictionary/Inflector.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <string>
#include <optional>
#include <string_view>

namespace inflection::grammar::synthesis {

FrGrammarSynthesizer_CountLookupFunction::FrGrammarSynthesizer_CountLookupFunction()
    : super(::inflection::util::LocaleUtils::FRENCH(), {u"singular", u"plural"}, {u"noun", u"adjective"})
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRENCH()))
    , dictionary(getDictionary())
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::FRENCH()))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nounProperty, {u"noun"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&properNounProperty, {u"proper-noun"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&appleProductProperty, {u"appleproduct"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&pluralizeInvProperty, {u"plural", u"singular"}));
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
        const ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(firstWord)));
        firstWord = npc(npc(tokenChain->getHead())->getNext())->getValue();
        if (firstWord.length() != word.length()) {
            result = determineWord(firstWord);
            if (result) {
                return *result;
            }
        }
    }

    if (dictionary.hasAllProperties(firstWord, nounProperty)) {
        if (::inflection::util::StringViewUtils::endsWith(firstWord, u"s") || ::inflection::util::StringViewUtils::endsWith(firstWord, u"x")) {
            return GrammemeConstants::NUMBER_PLURAL();
        } else {
            return GrammemeConstants::NUMBER_SINGULAR();
        }
    }
    return {};
}

} // namespace inflection::grammar::synthesis
