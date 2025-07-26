/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>

#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
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
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&pluralizeInvProperty, {u"plural", u"singular"}));
}

FrGrammarSynthesizer_CountLookupFunction::~FrGrammarSynthesizer_CountLookupFunction()
{
}

bool FrGrammarSynthesizer_CountLookupFunction::checkInvariantNouns(::std::u16string_view word, int64_t wordGrammemes) const
{
    if ((wordGrammemes & pluralizeInvProperty) == pluralizeInvProperty) {
        std::vector<dictionary::Inflector_InflectionPattern> inflectionPatterns;
        inflector.getInflectionPatternsForWord(word, inflectionPatterns);
        for (const auto& pattern : inflectionPatterns) {
            auto inflections(pattern.inflectionsForSurfaceForm(word, wordGrammemes));
            if (inflections.size() <= 1) {
                return true;
            }
            // There are at least 2 inflections to consider.
            int64_t patternGrammemes = 0;
            for (const auto& inflection : inflections) {
                patternGrammemes |= inflection.getGrammemes();
            }
            if ((patternGrammemes & pluralizeInvProperty) == pluralizeInvProperty != 0) {
                // We got patterns where all inflections that match the word suffix are both singular and plural.
                // Sometimes words can be invariant for one gender, but not another gender.
                return true;
            }
        }
    }
    return false;
}

::std::optional<::std::u16string> FrGrammarSynthesizer_CountLookupFunction::determineWord(const std::u16string &word) const {
    auto out = super::determine(word);
    if (!out.empty()) {
        return out;
    }
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, word);
    if ((wordGrammemes & properNounProperty) == properNounProperty) {
        return {{}};
    }
    if (checkInvariantNouns(word, wordGrammemes)) {
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
        if (firstWord.ends_with(u"s") || firstWord.ends_with(u"x")) {
            return GrammemeConstants::NUMBER_PLURAL();
        } else {
            return GrammemeConstants::NUMBER_SINGULAR();
        }
    }
    return {};
}

} // namespace inflection::grammar::synthesis
