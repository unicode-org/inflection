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

inline void traceLogSortedInflectionGrammemes(const ::std::vector<::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes> &inflectionGrammemes, const dictionary::DictionaryMetaData& dictionary) {
    std::u16string logStream(u"Sorted Inflection Grammemes:\n");
    for (const auto& inflectionCandidate: inflectionGrammemes) {
        logStream.append(u"\t\t").append(inflectionCandidate.getDescription(dictionary)).append(u"\n");
    }
    Logger::trace(logStream);
}

}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWordImplementation(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    if (LoggerConfig::isTraceEnabled()) {
        traceLogInflectCall(u"DictionaryLookupInflector::inflectWord", word, constraints, optionalConstraints, disambiguationGrammemeValues);
    }

    // Word not in dictionary
    if (wordGrammemes == 0) {
        if (LoggerConfig::isTraceEnabled()) {
            Logger::trace(std::u16string(word) + u": not in dictionary return std::nullopt\n");
        }
        return {};
    }

    ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    getInflector().getInflectionPatternsForWord(word, inflectionPatterns);
    if (inflectionPatterns.empty()) {
        if (LoggerConfig::isTraceEnabled()) {
            Logger::trace(u"\t" + std::u16string(word) + u": no inflection patterns found\n");
        }
        return {};
    }
    ::std::vector<::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes> inflectionGrammemes;
    filterInflectionGrammemes(word, wordGrammemes, inflectionPatterns, inflectionGrammemes);

    // No inflection patterns found
    if (inflectionGrammemes.empty()) {
        return {};
    }

    const auto &dictionary = getDictionary();
    ::std::vector<int64_t> disambiguationGrammemes;
    disambiguationGrammemes.reserve(disambiguationGrammemeValues.size());
    for (const auto &disambiguationGrammemeValue : disambiguationGrammemeValues) {
        disambiguationGrammemes.push_back(dictionary.getBinaryProperties({disambiguationGrammemeValue}));
    }

    if (inflectionGrammemes.size() > 1) {
        const auto inflectionComparator = [&](const InflectionGrammemes& inflection1, const InflectionGrammemes& inflection2) {
            return DictionaryLookupInflector::compareInflectionGrammemes(inflection1, inflection2, disambiguationGrammemes) < 0;
        };

        std::ranges::sort(inflectionGrammemes, inflectionComparator);
        if (LoggerConfig::isTraceEnabled()) {
            traceLogSortedInflectionGrammemes(inflectionGrammemes, dictionary);
        }
    }

    for (const auto &inflectionCandidate : inflectionGrammemes) {
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

        auto inflectedWord(inflection.getInflectionPattern().reinflectWithOptionalConstraints(inflection.getGrammemes(), dictionary.getBinaryProperties(constraints), optionalConstraintGrammemes, word));

        if (!inflectedWord.empty()) {
            return inflectedWord;
        }
        if (!enableDictionaryFallback) {
            break;
        }
    }
    return {};
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectImplementation(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    if (LoggerConfig::isTraceEnabled()) {
        traceLogInflectCall(u"DictionaryLookupInflector::inflect", word, constraints, optionalConstraints, disambiguationGrammemeValues);
    }
    // Constraints are empty
    if (std::ranges::all_of(constraints, [](const auto &x){ return x.empty(); })) {
        return std::u16string(word);
    }
    const bool allCaps = inflection::util::StringViewUtils::isAllUpperCase(word);
    // Never try to inflect all caps as is, we face issues like:
    // BIENVENU matching as bienvenu in the dictionary which has the inflection "" -> "e" which when applied to BIENVENU returns "BIENVENUe"
    if (!allCaps) {
        auto inflectedWord = inflectWordImplementation(word, wordGrammemes, constraints, optionalConstraints, disambiguationGrammemeValues);
        if (inflectedWord.has_value()) {
            return inflectedWord;
        }
    }
    const auto locale = getLocale();
    ::std::u16string lowerCasedWord;
    ::inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, locale);
    auto inflectedWord = inflectWordImplementation(lowerCasedWord, wordGrammemes, constraints, optionalConstraints, disambiguationGrammemeValues);
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

::std::optional<::std::u16string> DictionaryLookupInflector::inflect(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectImplementation(word, wordGrammemes, constraints, {}, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWithOptionalConstraints(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectImplementation(word, wordGrammemes, constraints, optionalConstraints, disambiguationGrammemeValues);
}

::std::optional<::std::u16string> DictionaryLookupInflector::inflectWord(std::u16string_view word, int64_t wordGrammemes, const ::std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const {
    return inflectWordImplementation(word, wordGrammemes, constraints, {}, disambiguationGrammemeValues);
}

int64_t DictionaryLookupInflector::disambiguationMatchScore(int64_t grammemes, const std::vector<int64_t> &disambiguationGrammemes) {
    int64_t result = 0;
    for (const auto disambiguationGrammeme : disambiguationGrammemes) {
        result = (result<<1) | (((grammemes & disambiguationGrammeme) != 0) ? 1 : 0);
    }
    return result;
}

int8_t DictionaryLookupInflector::compareInflectionGrammemes(const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes1, const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes2, const std::vector<int64_t> &disambiguationGrammemes) const {
    const int64_t grammemes1 = inflectionGrammemes1.grammemes;
    const int64_t grammemes2 = inflectionGrammemes2.grammemes;

    const int64_t disambgMatchScore1 = disambiguationMatchScore(grammemes1, disambiguationGrammemes);
    const int64_t disambgMatchScore2 = disambiguationMatchScore(grammemes2, disambiguationGrammemes);

    if (disambgMatchScore1 != disambgMatchScore2) {
        return (disambgMatchScore1 > disambgMatchScore2) ? -1 : 1;
    }

    const int8_t grammemesComparisonResult = compareGrammemes(grammemes1, grammemes2);

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

    auto num1 = std::popcount((uint64_t)grammemes1);
    auto num2 = std::popcount((uint64_t)grammemes2);
    if (num1 != num2) {
        return (num1 < num2) ? -1 : 1;
    }
    const int64_t cnt1 = inflectionOpt1->getInflectionPattern().numInflections();
    const int64_t cnt2 = inflectionOpt2->getInflectionPattern().numInflections();
    if (cnt1 != cnt2) {
        return (cnt1 < cnt2) ? 1 : -1;
    }
    return 0;
}

} // namespace inflection::dialog
