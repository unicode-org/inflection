/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/Logger.hpp>
#include <algorithm>
#include <bit>
#include <string>
#include <unicode/uchar.h>

namespace inflection::dialog {

using ::inflection::util::LoggerConfig;
using ::inflection::util::Logger;

DictionaryLookupInflector::DictionaryLookupInflector(const ::inflection::util::ULocale &locale, const std::vector<::std::vector<std::u16string_view>> &propertyPrioritiesData, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets, bool enableDictionaryFallback)
    : super(locale, {}, propertyPrioritiesData, ignoreGrammemeSets)
    , enableDictionaryFallback(enableDictionaryFallback)
{
}

DictionaryLookupInflector::~DictionaryLookupInflector()
{
}

namespace {

inline void traceLogInflectCall(const std::u16string &funcName, std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) {
    Logger::trace(funcName + u"("
        + u"word=" + std::u16string(word) + u", "
        + u"constraints = [" + inflection::util::StringViewUtils::join(constraints, u", ") + u"], "
        + u"optionalConstraints = [" + inflection::util::StringViewUtils::join(optionalConstraints, u", ") + u"], "
        + u"disambiguationGrammemeValues = [" + inflection::util::StringViewUtils::join(disambiguationGrammemeValues, u", ") + u"]"
        + u")\n");
}

inline void traceLogSortedInflectionGrammemes(const ::std::vector<::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes> &inflectionGrammemes, const ::std::vector<int64_t> &inflectionCandidates, const dictionary::DictionaryMetaData& dictionary) {
    std::u16string logStream(u"Sorted Inflection Grammemes:\n");
    for (const auto inflectionCandidate: inflectionCandidates) {
        logStream.append(u"\t\t").append(inflectionGrammemes.at(inflectionCandidate).getDescription(dictionary)).append(u"\n");
    }
    Logger::trace(logStream);
}

}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWordImplementation(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    if (LoggerConfig::isTraceEnabled()) {
        traceLogInflectCall(u"DictionaryLookupInflector::inflectWord", word, constraints, optionalConstraints, disambiguationGrammemeValues);
    }

    const auto &dictionary = getDictionary();
    const auto &inflector = getInflector();
    int64_t wordGrammemesMask = 0;

    // Word not in dictionary
    if (dictionary.getCombinedBinaryType(&wordGrammemesMask, word) == nullptr) {
        if (LoggerConfig::isTraceEnabled()) {
            Logger::trace(std::u16string(word) + u": not in dictionary return std::nullopt\n");
        }
        return {};
    }

    ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    inflector.getInflectionPatternsForWord(word, inflectionPatterns);
    if (inflectionPatterns.empty()) {
        if (LoggerConfig::isTraceEnabled()) {
            Logger::trace(u"\t" + std::u16string(word) + u": no inflection patterns found\n");
        }
        return {};
    }
    ::std::vector<::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes> inflectionGrammemes;
    getInflectionGrammemes(word, wordGrammemesMask, inflectionPatterns, inflectionGrammemes);

    //No inflection patterns found
    if (inflectionGrammemes.empty()) {
        return {};
    }
    filterInflectionGrammemes(inflectionGrammemes);
    ::std::vector<int64_t> disambiguationGrammemes;
    disambiguationGrammemes.reserve(disambiguationGrammemeValues.size());
    for (const auto &disambiguationGrammemeValue : disambiguationGrammemeValues) {
        disambiguationGrammemes.push_back(dictionary.getBinaryProperties({disambiguationGrammemeValue}));
    }

    ::std::vector<int64_t> inflectionIdxs(inflectionGrammemes.size());
    for (int64_t i=0 ; i < (int64_t) inflectionGrammemes.size() ; i++) {
        inflectionIdxs[i] = i;
    }
    const auto inflectionComparator = [&](const int64_t idx1, const int64_t idx2) {
        return DictionaryLookupInflector::compareInflectionGrammemes(inflectionGrammemes.at(idx1), inflectionGrammemes.at(idx2), disambiguationGrammemes) < 0;
    };

    ::std::vector<int64_t> inflectionCandidates;
    if (!enableDictionaryFallback) {
        inflectionCandidates.emplace_back(*::std::min_element(inflectionIdxs.begin(), inflectionIdxs.end(), inflectionComparator));
    } else {
        inflectionCandidates = inflectionIdxs;
        ::std::sort(inflectionCandidates.begin(), inflectionCandidates.end(), inflectionComparator);
        if (LoggerConfig::isTraceEnabled()) {
            traceLogSortedInflectionGrammemes(inflectionGrammemes, inflectionCandidates, dictionary);
        }
    }

    for (const auto &inflectionCandidateIdx : inflectionCandidates) {
        const auto &inflectionCandidate = inflectionGrammemes.at(inflectionCandidateIdx);
        if (!inflectionCandidate.inflection.has_value()) {
            return std::u16string(word);
        }
        const auto &inflection = inflectionCandidate.inflection.value();
        if (LoggerConfig::isTraceEnabled()) {
            Logger::trace(std::u16string(u"reinflect function called:\n\targs:")
                          + u"\n\t\tword:"
                          + std::u16string(word)
                          + u"\n\t\tinflectionGrammemes: "
                          + inflection::util::StringViewUtils::join(dictionary.getPropertyNames(inflection.getGrammemes()), u", ")
                          + u"\n\t\tconstraints: "
                          + inflection::util::StringViewUtils::join(constraints, u", ")
                          + u"\n\t\toptionalConstraints: "
                          + inflection::util::StringViewUtils::join(optionalConstraints, u", ")
                          + u"\n");
        }
        
        std::vector<int64_t> optionalConstraintGrammemes;
        optionalConstraintGrammemes.reserve(optionalConstraints.size());
        for (const auto &optionalConstraint: optionalConstraints) {
            optionalConstraintGrammemes.emplace_back(dictionary.getBinaryProperties({optionalConstraint}));
        }
        
        const auto &inflectedWord = inflection.getInflectionPattern()->reinflectWithOptionalConstraints(inflection.getGrammemes(), dictionary.getBinaryProperties(constraints), optionalConstraintGrammemes, word);

        if (!inflectedWord.empty()) {
            return inflectedWord;
        }
    }
    return {};
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectImplementation(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    if (LoggerConfig::isTraceEnabled()) {
        traceLogInflectCall(u"DictionaryLookupInflector::inflect", word, constraints, optionalConstraints, disambiguationGrammemeValues);
    }
    // Constraints are empty
    if (::std::all_of(constraints.begin(), constraints.end(), [](const auto &x){ return x.empty(); })) {
        return std::u16string(word);
    }
    const bool allCaps = inflection::util::StringViewUtils::isAllUpperCase(word);
    // Never try to inflect all caps as is, we face issues like:
    // BIENVENU matching as bienvenu in the dictionary which has the inflection "" -> "e" which when applied to BIENVENU returns "BIENVENUe"
    if (!allCaps){
        auto inflectedWord = inflectWordImplementation(word, constraints, optionalConstraints, disambiguationGrammemeValues);
        if (inflectedWord.has_value()) {
            return inflectedWord;
        }
    }
    const auto locale = getLocale();
    ::std::u16string lowerCasedWord;
    ::inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, locale);
    auto inflectedWord = inflectWordImplementation(lowerCasedWord, constraints, optionalConstraints, disambiguationGrammemeValues);
    if (inflectedWord.has_value()) {
        //If word all caps then make inflection all upper case
        if (allCaps) {
            ::std::u16string upperCaseInflectedWord;
            ::inflection::util::StringViewUtils::uppercase(&upperCaseInflectedWord, *inflectedWord, locale);
            inflectedWord = upperCaseInflectedWord;
        } else if ((bool)u_isupper((UChar32) ::inflection::util::StringViewUtils::codePointAt(word, 0))) {
            //If word is capitalized first then inflection is capitalized first
            inflectedWord = inflection::util::StringViewUtils::capitalizeFirst(*inflectedWord, locale);
        }
        return inflectedWord;
    }
    return {};
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflect(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectImplementation(word, constraints, {}, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWithOptionalConstraints(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectImplementation(word, constraints, optionalConstraints, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWord(std::u16string_view word, const ::std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectWordImplementation(word, constraints, {}, disambiguationGrammemeValues);
}

int64_t DictionaryLookupInflector::disambiguationMatchScore(int64_t grammemes, const std::vector<int64_t> &disambiguationGrammemes) const{
    int64_t result = 0;
    for (const auto disambiguationGrammeme : disambiguationGrammemes) {
        result = (result<<1) | (((grammemes & disambiguationGrammeme) != 0) ? 1 : 0);
    }
    return result;
}

int64_t DictionaryLookupInflector::compareInflectionGrammemes(const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes1, const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes2, const std::vector<int64_t> &disambiguationGrammemes) const {
    const int64_t grammemes1 = inflectionGrammemes1.grammemes;
    const int64_t grammemes2 = inflectionGrammemes2.grammemes;

    const int64_t disambgMatchScore1 = disambiguationMatchScore(grammemes1, disambiguationGrammemes);
    const int64_t disambgMatchScore2 = disambiguationMatchScore(grammemes2, disambiguationGrammemes);

    if (disambgMatchScore1 != disambgMatchScore2) {
        return ((disambgMatchScore1 > disambgMatchScore2) ? -1 : 1);
    }

    const int64_t grammemesComparisonResult = compareGrammemes(grammemes1, grammemes2);

    if (grammemesComparisonResult != 0) {
        return grammemesComparisonResult;
    }

    const auto &inflectionOpt1 = inflectionGrammemes1.inflection;
    const auto &inflectionOpt2 = inflectionGrammemes2.inflection;
    if (!inflectionOpt1.has_value()) {
        return 1;
    }
    if (!inflectionOpt2.has_value()) {
        return -1;
    }
    const auto &inflection1 = inflectionOpt1.value();
    const auto &inflection2 = inflectionOpt2.value();

    auto num1 = std::popcount((uint64_t)grammemes1);
    auto num2 = std::popcount((uint64_t)grammemes2);
    if (num1 != num2) {
        return (num1 < num2) ? -1 : 1;
    }
    const int64_t cnt1 = inflection1.getInflectionPattern()->numInflections();
    const int64_t cnt2 = inflection2.getInflectionPattern()->numInflections();
    if (cnt1 != cnt2) {
        return (cnt1 < cnt2) ? 1 : -1;
    }
    return 0;
}

} // namespace inflection::dialog
