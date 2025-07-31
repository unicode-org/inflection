/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>

#include <inflection/dictionary/Inflector_Inflection.hpp>
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/Logger.hpp>
#include <algorithm>
#include <bit>
#include <string>
#include <vector>
#include <compare>

namespace inflection::dictionary {

Inflector_InflectionPattern::Inflector_InflectionPattern(int32_t identifierID, int32_t frequencyIdx, int64_t partsOfSpeech, int16_t lemmaSuffixesLen,
                                                         int32_t lemmaSuffixesOffset, int16_t numOfInflections, int32_t inflectionsArrayStart,
                                                          const inflection::dictionary::Inflector_MMappedDictionary& containingDictionary)
    : identifierID(identifierID)
    , frequencyIdx(frequencyIdx)
    , partsOfSpeech(partsOfSpeech)
    , lemmaSuffixesLen(lemmaSuffixesLen)
    , numOfInflections(numOfInflections)
    , lemmaSuffixesOffset(lemmaSuffixesOffset)
    , inflectionsArrayStart(inflectionsArrayStart)
    , inflectorDictionary(containingDictionary)
{
}

::std::vector<Inflector_Inflection> Inflector_InflectionPattern::constrain(const ::std::vector<::std::u16string>& constraints) const
{
    return constrain(constraints, true);
}

inline static bool containsAll(int64_t superset, int64_t subset){
    return (superset & subset) == subset;
}

Inflector_Inflection inflection::dictionary::Inflector_InflectionPattern::getInflectionAtPosition(int32_t idx) const {
    if (idx >= numOfInflections) {
        throw inflection::exception::IndexOutOfBoundsException(u"index too large for getInflectionAtPosition");
    }
    uint64_t value = inflectorDictionary.inflectionsArray.read(inflectionsArrayStart + idx);
    auto grammemesIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(value, 0, inflectorDictionary.numBitsForGrammemesIdx);
    value >>= inflectorDictionary.numBitsForGrammemesIdx;
    auto suffixIdx = inflection::dictionary::metadata::CompressedArray<int32_t>::extractValue(value, 0, inflectorDictionary.numBitsForSuffixIdx);
    // In the future, the prefixIdx could be used, and that would be for prefixes. The numBitsForPrefixIdx has the number of bits.
    return {
        *this,
        int32_t(suffixIdx),
        *(inflectorDictionary.grammemePatterns + grammemesIdx)
    };
}

bool Inflector_InflectionPattern::containsSuffix(std::u16string_view suffix) const
{
    for (int16_t i = 0; i < numOfInflections; ++i) {
        if (getInflectionAtPosition(i).getSuffix() == suffix) {
            return true;
        }
    }
    return false;
}

::std::vector<Inflector_Inflection> Inflector_InflectionPattern::constrain(const ::std::vector<::std::u16string>& constraints, bool isSuperset) const {
    int64_t constraintGrammemes = inflectorDictionary.dictionary.getValuesOfTypes(constraints);
    ::std::vector<Inflector_Inflection> results;
    for (int16_t i = 0; i < numOfInflections; ++i) {
        const auto &inflection = getInflectionAtPosition(i);
        int64_t inflectionGrammemes = inflection.getGrammemes();

        if (isSuperset ? containsAll(constraintGrammemes, inflectionGrammemes) : containsAll(inflectionGrammemes, constraintGrammemes)) {
            results.push_back(inflection);
        }
    }
    return results;
}

void Inflector_InflectionPattern::getLemmaSuffixMatches(::std::vector<Inflector_Inflection>& results) const
{
    results.clear();
    for (int16_t i = 0; i < numOfInflections; ++i) {
        const auto inflection(getInflectionAtPosition(i));

        for (int16_t i = 0; i < lemmaSuffixesLen; ++i) {
            auto currLemmaSuffixId = static_cast<int32_t>(
                inflectorDictionary.inflectionsArray.read(lemmaSuffixesOffset + i));
            if (currLemmaSuffixId == inflection.suffixId) {
                // We matched a lemma suffix. Add it.
                results.emplace_back(inflection);
                break;
            }
        }
    }
}

::std::vector<Inflector_Inflection> Inflector_InflectionPattern::inflectionsForSurfaceForm(::std::u16string_view surfaceForm, int64_t fromGrammemes) const {
    ::std::vector<Inflector_Inflection> results;
    int64_t maxLen = -1;
    for (int16_t i = 0; i < numOfInflections; ++i) {
        const auto inflection(getInflectionAtPosition(i));
        int64_t inflectionGrammemes = inflection.getGrammemes();
        if (!containsAll(fromGrammemes, inflectionGrammemes)) {
            continue;
        }
        const auto suffix(inflection.getSuffix());
        auto sufLen = (int64_t) suffix.size();
        if (sufLen < maxLen || !surfaceForm.ends_with(suffix)) {
            continue;
        }
        if (sufLen > maxLen) {
            results.clear();
            maxLen = sufLen;
        }
        results.push_back(inflection);
    }
    return results;
}

::std::optional<Inflector_Inflection> Inflector_InflectionPattern::getMatchingLemmaInflection(const ::std::vector<Inflector_Inflection>& inflections) const
{
    if (inflections.size() > 1) {
        std::u16string lemmaSuffix;
        int32_t shortestLen = INT32_MAX;
        int32_t shortestIndex = 0;
        int32_t index = 0;
        /* In this scenario we have a list of inflections that are scored equally.
         * We have to choose one.
         * The first inflection should be the most preferred because the rare forms are sorted last,
         * but perhaps there are alternate spellings that has no sorting order.
         * Bias the result towards one that matches the main lemma suffix.
         */
        if (lemmaSuffixesLen <= 1) {
            if (lemmaSuffixesLen == 1) {
                lemmaSuffix = inflectorDictionary.inflectionSuffixes.getString(static_cast<int32_t>(
                    inflectorDictionary.inflectionsArray.read(lemmaSuffixesOffset)));
            }
            // else lemmaSuffix is an empty string.
            for (const auto &inflection : inflections) {
                const std::u16string suffix(inflection.getSuffix());
                if (static_cast<int32_t>(suffix.length()) < shortestLen) {
                    shortestLen = static_cast<int32_t>(suffix.length());
                    shortestIndex = index;
                }
                if (lemmaSuffix == suffix) {
                    // We matched a lemma suffix after the first attempt to match, we can't get better.
                    return inflection;
                }
                index++;
            }
        }
        else {
            // This is the slow loop that calls getSuffix many times. Most languages, except Finnish, only have a single lemma suffix to consider.
            for (const auto &inflection : inflections) {
                const std::u16string suffix(inflection.getSuffix());
                if (static_cast<int32_t>(suffix.length()) < shortestLen) {
                    shortestLen = static_cast<int32_t>(suffix.length());
                    shortestIndex = index;
                }
                for (int16_t i = 0; i < lemmaSuffixesLen; ++i) {
                    lemmaSuffix = inflectorDictionary.inflectionSuffixes.getString(static_cast<int32_t>(
                        inflectorDictionary.inflectionsArray.read(lemmaSuffixesOffset + i)));
                    if (lemmaSuffix == suffix) {
                        // We matched a lemma suffix, and we can't get better.
                        return inflection;
                    }
                }
                index++;
            }
        }
        return inflections[shortestIndex];
    }
    // We have a single match.
    return inflections.front();
}

::std::optional<Inflector_Inflection> Inflector_InflectionPattern::selectLemmaInflection(int64_t fromGrammemes, const ::std::vector<int64_t> &lemmaAttributes) const {
    if (numOfInflections == 0) {
        return {};
    }
    ::std::vector<Inflector_Inflection> selectedInflections;
    std::tuple<int64_t, int16_t, int16_t> selectedTuple(-1, 0, -UINT8_MAX);
    //Represents the tuple score, max inflection grammemes matched with word grammemes, min number of inflection grammemes, minimum suffix length
    for (int16_t i = 0; i < numOfInflections; ++i) {
        const auto inflection = getInflectionAtPosition(i);
        int64_t inflectionGrammemes = inflection.getGrammemes();
        int64_t score = 0;
        for (const auto lemmaAttribute : lemmaAttributes) {
            score = (2 * score) + (((inflectionGrammemes & lemmaAttribute) != 0) ? 1 : 0);
        }
        const auto numberOfMatches = std::popcount(uint64_t(inflectionGrammemes & fromGrammemes));
        const auto grammemesLen = std::popcount(uint64_t(inflectionGrammemes));

        const std::tuple<int64_t, int16_t, int16_t> candidateTuple(score, numberOfMatches, -grammemesLen);
        if (candidateTuple < selectedTuple) {
            continue;
        }
        if (candidateTuple > selectedTuple) {
            selectedTuple = candidateTuple;
            selectedInflections.clear();
        }
        selectedInflections.emplace_back(inflection);
    }
    if (!selectedInflections.empty()) {
        return getMatchingLemmaInflection(selectedInflections);
    }
    return {};
}

//1. fromGrammemes, toConstraints can have empty values
std::u16string Inflector_InflectionPattern::reinflectImplementation(int64_t fromGrammemes, int64_t toConstraints, const std::vector<int64_t> &toOptionalConstraints, std::u16string_view surfaceForm) const
{
    if (toConstraints == 0) {
        // That's a confusing request. It implies that anything is acceptable because it's unconstrained. So use the one provided.
        return ::std::u16string(surfaceForm);
    }
    // Checks if toConstraint is subset of from grammemes
    if (containsAll(fromGrammemes, toConstraints)) {
        // What you're inflecting to is already the word properties. So don't change anything.
        return ::std::u16string(surfaceForm);
    }

    int16_t longestLemmaSuffixLen = 0;
    std::optional<std::u16string> bestSurfaceFormSuffix;
    
    struct SurfaceFormMatchScore {
        int64_t optionalConstraintsMatchScore = -1;
        int64_t surfaceFormGrammemesMatched = -1;
        int64_t surfaceFormGrammemesUnmatchedNegative = INT32_MIN;
        auto operator<=>(const SurfaceFormMatchScore&) const = default;
    };
    
    SurfaceFormMatchScore surfaceFormMatchScore;

    for (int16_t i = 0; i < numOfInflections; ++i) {
        const auto& inflection = getInflectionAtPosition(i);
        int64_t inflectionGrammemes = inflection.getGrammemes();
        // These surfaceForm grammeme should have been derived from the dictionary entry.
        
        // If the current surfaceForm grammemes are "masculine, singular",
        // don't consider "masculine, plural", "feminine, singular" nor "masculine, singular, genitive".
        
        // If the current surfaceForm grammemes are "feminine, masculine, singular",
        // do consider "masculine, singular" and "feminine, singular" but not anything with "plural".
        
        // If the current surfaceForm grammemes are unknown, make the best guess.
        const std::u16string surfaceFormSuffix(inflection.getSuffix());
        if ((fromGrammemes == 0) || containsAll(fromGrammemes, inflectionGrammemes)) {
            auto surfaceFormSuffixSize = (int16_t) surfaceFormSuffix.size();
            if (longestLemmaSuffixLen < surfaceFormSuffixSize && surfaceForm.ends_with(surfaceFormSuffix)) {
                longestLemmaSuffixLen = surfaceFormSuffixSize;
            }
        }
        if (containsAll(inflectionGrammemes, toConstraints)) {
            // At this point, it's not a no, but it's not the best either.
            // Perhaps you asked for the "plural" form, but there are both "masculine" and "feminine" forms.
            // Perhaps you asked for the "plural, feminine" form, but there are both "nominative" and "genitive" forms.
            // Try to find one where the fewest unreferenced grammemes are changing.
            const auto numberOfMatches = std::popcount(uint64_t(inflectionGrammemes & fromGrammemes));
            int64_t optionalConstraintsMatchScore = 0;
            for (const auto constraintGrammeme : toOptionalConstraints) {
                optionalConstraintsMatchScore = 2 * optionalConstraintsMatchScore + (((inflectionGrammemes & constraintGrammeme) != 0) ? 1 : 0);
            }
            
            const auto currentUnmatchedNegative = -(std::popcount(uint64_t(inflectionGrammemes)) - std::popcount(uint64_t(toConstraints)));
            
            const SurfaceFormMatchScore currentSurfaceFormMatchScore{optionalConstraintsMatchScore, numberOfMatches, currentUnmatchedNegative};

            if (util::LoggerConfig::isTraceEnabled()) {
                // Logging reinflect inflection candidate
                util::Logger::trace(std::u16string(u"reinflect result candidate suffix: ")
                          + std::u16string(surfaceFormSuffix) + u" , inflectionGrammemes: ["
                          + inflection::util::StringViewUtils::join(inflectorDictionary.dictionary.getTypesOfValues(inflectionGrammemes), u", ")
                          + u"], Number of matches with existing grammemes: " + util::StringUtils::to_u16string(numberOfMatches)
                                    + u", Optional Constraint Match Score: " + util::StringUtils::to_u16string(optionalConstraintsMatchScore));
            }
            if (currentSurfaceFormMatchScore > surfaceFormMatchScore) {
                surfaceFormMatchScore = currentSurfaceFormMatchScore;
                bestSurfaceFormSuffix = surfaceFormSuffix;
            }
        }
    }
    if (bestSurfaceFormSuffix.has_value()) {
        return ::std::u16string(surfaceForm.substr(0, surfaceForm.size() - longestLemmaSuffixLen)) + bestSurfaceFormSuffix.value();
    }
    // It just doesn't exist. Perhaps it's uninflectable, like sheep or news.
    return {};
}

::std::u16string Inflector_InflectionPattern::reinflect(int64_t fromGrammemes, int64_t toConstraints, std::u16string_view surfaceForm) const {
    return reinflectImplementation(fromGrammemes, toConstraints, {}, surfaceForm);
}

::std::u16string Inflector_InflectionPattern::reinflectWithOptionalConstraints(int64_t fromGrammemes, int64_t toConstraints, const std::vector<int64_t> &toOptionalConstraints, std::u16string_view surfaceForm) const {
    return reinflectImplementation(fromGrammemes, toConstraints, toOptionalConstraints, surfaceForm);
}

::std::u16string Inflector_InflectionPattern::getIdentifier() const
{
    return inflectorDictionary.identifierToInflectionPatternTrie.getKey(identifierID);
}

int32_t Inflector_InflectionPattern::getFrequency() const
{
    return inflectorDictionary.frequenciesArray[frequencyIdx];
}

bool Inflector_InflectionPattern::containsPartsOfSpeech(const ::std::u16string& pos) const
{
    const auto posMask = inflectorDictionary.dictionary.getValueOfType(pos).value_or(0);
    return (partsOfSpeech & posMask) != 0;
}

int32_t Inflector_InflectionPattern::numInflections() const
{
    return numOfInflections;
}

std::weak_ordering Inflector_InflectionPattern::operator<=>(const Inflector_InflectionPattern& other) const {
    return getIdentifier() <=> other.getIdentifier();
}

int64_t Inflector_InflectionPattern::firstContainingPartOfSpeech(const ::std::vector<int64_t> &partOfSpeechList) const {
    int64_t partsOfSpeechLocal = getPartsOfSpeech();
    return ::std::distance(partOfSpeechList.begin(), ::std::find_if(partOfSpeechList.begin(), partOfSpeechList.end(), [partsOfSpeechLocal](int64_t partOfSpeech){
        return ((partsOfSpeechLocal & partOfSpeech) != 0);
    }));
}

int64_t Inflector_InflectionPattern::getPartsOfSpeech() const {
    return partsOfSpeech;
}

} // namespace inflection::dictionary
