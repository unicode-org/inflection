/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_LocativeBasedLookupFunction.hpp>

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_FiDisplayFunction.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

FiGrammarSynthesizer_LocativeBasedLookupFunction::FiGrammarSynthesizer_LocativeBasedLookupFunction(const FiGrammarSynthesizer_FiDisplayFunction& displayFunction, FiGrammarSynthesizer::Case caseForInner, FiGrammarSynthesizer::Case caseForOuter)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::FINNISH())))
    , displayFunction(displayFunction)
    , caseForOuter(caseForOuter)
    , caseForInner(caseForInner)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&nouns,  {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_PROPER_NOUN}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&pluralProperty, {GrammemeConstants::NUMBER_PLURAL}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&outerProperty, {FiGrammarSynthesizer::OUTER}));
    // verb participles act like adjectives.
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&adjectiveProperty, {GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::VERBTYPE_PARTICIPLE}));
}

inflection::dialog::SpeakableString* FiGrammarSynthesizer_LocativeBasedLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string locationString;
    auto isCommaSeparated = displayString.find(u',') != std::u16string::npos;
    ::std::vector<::std::u16string> words(::inflection::util::StringViewUtils::split(::inflection::util::StringViewUtils::trim(displayString), u" "));
    if (isCommaSeparated || words.size() > 1) {
        // So the display string has multiple whitespace-separated words, and we have to figure what parts
        // should be inflected: i.e. in "Tampere Suomi" we have to inflect both words and in "New York" we
        // have to inflect it as one word.
        ::std::vector<::std::u16string> finalCombination;
        if (isCommaSeparated) {
            finalCombination.reserve(words.size());
            for (::inflection::util::DelimitedStringIterator iterator(displayString, u","); iterator.hasNext(); ++iterator) {
                finalCombination.emplace_back(::inflection::util::StringViewUtils::trim(*iterator));
            }
        }
        else {
            ::std::vector<::std::vector<::std::u16string>> finalCombinationRaw;
            // Generate all possible combinations
            auto possibleCombinations = FiGrammarSynthesizer::getPossibleCombinations(words);
            // Filter out the combinations for which some part is not in the dictionary
            auto dictionaryCombinations = getDictionaryCombinations(possibleCombinations);
            if (dictionaryCombinations.empty()) {
                // All combinations have at least one part which is not in the dictionary. We'll take the
                // first of the possible combinations, i.e. the one in which all the parts form a single word.
                finalCombinationRaw = possibleCombinations.front();
            }
            else if (dictionaryCombinations.size() == 1) {
                finalCombinationRaw = dictionaryCombinations.front();
            }
            else {
                // We will choose the combination with the fewest parts. It should have (on average) the longest parts found in
                // dictionary. If there are ties, we take the first one
                finalCombinationRaw = dictionaryCombinations.front();
                for (const auto& possibleCombination : dictionaryCombinations) {
                    if (possibleCombination.size() < finalCombinationRaw.size()) {
                        finalCombinationRaw = possibleCombination;
                    }
                }
            }
            finalCombination.reserve(finalCombinationRaw.size());
            for (const auto& arr : finalCombinationRaw) {
                finalCombination.emplace_back(::inflection::util::StringViewUtils::join(arr, u" "));
            }
        }
        ::std::vector<::std::u16string> inflectedWords;
        inflectedWords.reserve(finalCombination.size());
        if (finalCombination.size() == 2) {
            // Support NPs like A + N
            const auto& attribute = finalCombination[0];
            const auto& head = finalCombination[1];
            if (dictionary.hasAnyProperty(attribute, nouns) && dictionary.hasAnyProperty(head, adjectiveProperty)) {
                // A noun-adjective phrase. Don't touch the noun.
                inflectedWords.emplace_back(attribute);
            }
            else {
                auto isAdjectiveNounPhrase = dictionary.hasAnyProperty(attribute, adjectiveProperty) && dictionary.hasAnyProperty(head, nouns);
                // The adjective attribute agrees to the inflection of the head
                inflectedWords.emplace_back(getLocativeString(attribute, isAdjectiveNounPhrase ? head : attribute));
            }
            inflectedWords.emplace_back(getLocativeString(head));
        } else {
            for (const auto& word : finalCombination) {
                inflectedWords.emplace_back(getLocativeString(word));
            }
        }
        locationString = ::inflection::util::StringViewUtils::join(inflectedWords, isCommaSeparated ? u", " : u" ");
    }
    else {
        locationString = getLocativeString(displayString);
    }
    if (locationString.empty() || ::inflection::util::StringViewUtils::trim(locationString).empty()) {
        locationString = displayString;
    }
    return new ::inflection::dialog::SpeakableString(locationString);
}

::std::vector<::std::vector<::std::vector<::std::u16string>>> FiGrammarSynthesizer_LocativeBasedLookupFunction::getDictionaryCombinations(const ::std::vector<::std::vector<::std::vector<::std::u16string>>>& possibleCombinations) const
{
    ::std::vector<::std::vector<::std::vector<::std::u16string>>> dictionaryCombinations;
    for (const auto& possibleCombination : possibleCombinations) {
        if (isInDictionary(possibleCombination)) {
            dictionaryCombinations.emplace_back(possibleCombination);
        }
    }
    return dictionaryCombinations;
}

bool FiGrammarSynthesizer_LocativeBasedLookupFunction::isInDictionary(const ::std::vector<::std::vector<::std::u16string>>& possibleCombination) const
{
    for (const auto& strings : possibleCombination) {
        ::std::u16string currentWord(::inflection::util::StringViewUtils::join(strings, u" "));
        if (!dictionary.isKnownWord(currentWord)) {
            return false;
        }
    }
    return true;
}

std::u16string FiGrammarSynthesizer_LocativeBasedLookupFunction::getLocativeString(const ::std::u16string& displayString) const
{
    return getLocativeString(displayString, displayString);
}

std::u16string FiGrammarSynthesizer_LocativeBasedLookupFunction::getLocativeString(const ::std::u16string& attributeDisplayString, const ::std::u16string& headDisplayString) const
{
    auto count = FiGrammarSynthesizer::Number::singular;
    if (dictionary.hasAllProperties(headDisplayString, pluralProperty)) {
        count = FiGrammarSynthesizer::Number::plural;
    }
    if (dictionary.hasAllProperties(headDisplayString, outerProperty)) {
        return displayFunction.inflectString(attributeDisplayString, GrammemeConstants::POS_NOUN, caseForOuter, count, FiGrammarSynthesizer::Person::undefined, true);
    } else {
        return displayFunction.inflectString(attributeDisplayString, GrammemeConstants::POS_NOUN, caseForInner, count, FiGrammarSynthesizer::Person::undefined, true);
    }
}

} // namespace inflection::grammar::synthesis
