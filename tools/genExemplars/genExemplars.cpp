/*
 * Copyright 2025-2026 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/ulocdata.h>
#include <unicode/uchar.h>
#include <ranges>
#include <iostream>
#include <fstream>

using namespace std;

static u16string findBestExemplar(const map<u16string, int32_t>& wordWeights, const set<u16string>& words) {
    int32_t shortestResultWeight = 0;
    u16string shortestResult;
    for (const auto& word : words) {
        if (shortestResult.empty() || word.length() < shortestResult.length()) {
            shortestResult = word;
            shortestResultWeight = 0;
            auto wordResult = wordWeights.find(word);
            if (wordResult != wordWeights.end()) {
                shortestResultWeight = wordResult->second;
            }
        }
        else if (word.length() == shortestResult.length()) {
            auto wordResult = wordWeights.find(word);
            if (wordResult != wordWeights.end() && wordResult->second >= shortestResultWeight && u_islower(word.front())) {
                shortestResultWeight = wordResult->second;
                shortestResult = word;
            }
        }
    }
    if (shortestResult.empty()) {
        return *words.begin();
    }
    return shortestResult;
}

typedef class WordInfo {
public:
    set<u16string> suffixes;
} WordInfo;

typedef class WordSuffixValue {
public:
    u16string id;
    u16string exemplar;
    WordSuffixValue(const u16string& id, const u16string& exemplar)
        : id(id), exemplar(exemplar)
    {
    }
} WordSuffixValue;

static icu4cxx::UnicodeSet getExemplarCharacters(const ::inflection::util::ULocale& locale) {
    icu4cxx::UnicodeSet exemplarSet;
    auto status = U_ZERO_ERROR;
    auto locData = ulocdata_open(locale.getName().c_str(), &status);
    ulocdata_getExemplarSet(locData, npc(exemplarSet.wrappee_), USET_CASE_INSENSITIVE, ULOCDATA_ES_STANDARD, &status);
    ulocdata_close(locData);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return exemplarSet;
}

static void generateTable(const inflection::util::ULocale& locale, const u16string& partsOfSpeech, const map<u16string, int32_t>& wordWeights, int32_t maximumUniqueFrequencies, bool verbose) {
    const inflection::dictionary::DictionaryMetaData& dictionary = *npc(inflection::dictionary::DictionaryMetaData::createDictionary(locale));
    const inflection::dictionary::Inflector& inflector(inflection::dictionary::Inflector::getInflector(locale));
    vector<inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    map<int32_t, map<u16string, map<int64_t, set<u16string>>>> frequencyToInflectionID;
    icu4cxx::UnicodeSet nativeSet(getExemplarCharacters(locale));

    int64_t posToMatch = 0;
    for (inflection::util::DelimitedStringIterator pos(partsOfSpeech, u","); pos.hasNext(); ++pos) {
        posToMatch |= dictionary.getBinaryProperties({u16string(*pos)});
    }

    for (const auto& word : dictionary.getKnownWords()) {
        inflectionPatterns.clear();
        inflector.getInflectionPatternsForWord(word, inflectionPatterns);
        if (inflectionPatterns.size() == 1 && nativeSet.contains(word.back())) {
            const auto& pattern = inflectionPatterns.front();
            auto frequency = pattern.getFrequency();
            if ((pattern.getPartsOfSpeech() & posToMatch) != 0
                && (static_cast<int32_t>(frequencyToInflectionID.size()) < maximumUniqueFrequencies || frequencyToInflectionID.begin()->first <= frequency))
            {
                int64_t grammemes = 0;
                dictionary.getCombinedBinaryType(&grammemes, word);
                frequencyToInflectionID[frequency][pattern.getIdentifier()][grammemes].insert(word);
                if (static_cast<int32_t>(frequencyToInflectionID.size()) > maximumUniqueFrequencies) {
                    frequencyToInflectionID.erase(frequencyToInflectionID.begin());
                }
            }
        }
    }

    int32_t cumulativeFrequencies = 0;
    int32_t maxSuffixLength = 0;
    map<u16string, WordSuffixValue> suffixesToGrammemesWithId;
    for (const auto& [frequency, inflectionIDs] : frequencyToInflectionID | views::reverse) {
        map<u16string, set<u16string>> presuffixExemplarMap;

        for (const auto& [id, grammemeMap] : inflectionIDs) {
            if (verbose) {
                cout << frequency << "," << inflection::util::StringViewUtils::to_string(id) << endl;
            }
            map<int64_t, WordInfo> grammemesToWords;
            auto pattern = *inflector.getInflectionPatternByName(id);
            auto baseGrammemes = pattern.getPartsOfSpeech();
            for (auto const& inflection : pattern.constrain({}, false)) {
                auto grammemesToMatch = inflection.getGrammemes() | baseGrammemes;
                for (const auto& [grammemes, words] : grammemeMap) {
                    auto suffix = inflection.getSuffix();
                    if ((grammemesToMatch & grammemes) == grammemesToMatch && words.begin()->ends_with(inflection.getSuffix())) {
                        grammemesToWords[grammemesToMatch].suffixes.emplace(suffix);
                        for (const auto& word : words) {
                            const auto ch = word[word.length() - suffix.length() - 1];
                            const char16_t lowerCh = static_cast<char16_t>(u_tolower(ch));
                            if (ch == lowerCh) {
                                for (int32_t presuffixLen = min(1, static_cast<int32_t>(word.length() - suffix.length() - 1)); presuffixLen >= 1; presuffixLen--) {
                                    u16string longerSuffix(word, word.length() - suffix.length() - presuffixLen, suffix.length() + presuffixLen);
                                    presuffixExemplarMap[longerSuffix].insert(word);
                                    if (static_cast<int32_t>(longerSuffix.length()) > maxSuffixLength) {
                                        maxSuffixLength = static_cast<int32_t>(longerSuffix.length());
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (verbose) {
                cout << "    presuffixSet: ";
                bool addComma = false;
                for (const auto& presuffix: presuffixExemplarMap | views::keys) {
                    if (addComma) {
                        cout << ",";
                    }
                    cout << inflection::util::StringViewUtils::to_string(presuffix);
                    addComma = true;
                }
                cout << endl;
            }
            for (const auto& [grammemes, wordMetadata] : grammemesToWords) {
                if (verbose) {
                    cout << "    ";
                }
                bool addComma = false;
                for (const auto& suffix : wordMetadata.suffixes) {
                    if (verbose) {
                        if (addComma) {
                            cout << ",";
                        }
                        addComma = true;
                        cout << inflection::util::StringViewUtils::to_string(suffix);
                    }
                    for (const auto& [presuffix, words] : presuffixExemplarMap) {
                        suffixesToGrammemesWithId.emplace(presuffix, WordSuffixValue(id, findBestExemplar(wordWeights, words)));
                    }
                }
                if (verbose) {
                    cout << ": ";
                    cout << inflection::util::StringViewUtils::to_string(inflection::util::StringViewUtils::join(dictionary.getPropertyNames(grammemes), u",")) << endl;
                }
            }
        }
        cumulativeFrequencies += frequency;
    }

    cout << "// cumulative frequencies: " << cumulativeFrequencies << endl;
    cout << "static constexpr int32_t MAX_SUFFIX_LEN = " << maxSuffixLength << ";" << endl;

    for (const auto& [suffix, grammemesWithId] : suffixesToGrammemesWithId) {
        cout << inflection::util::StringViewUtils::to_string(suffix)
             << ",=," << inflection::util::StringViewUtils::to_string(grammemesWithId.exemplar) << endl;
    }
}

static constexpr char USAGE_STRING[] =
        "Usage: genExemplars --locale LOCALE [--weights INFILE --verbose --top NUMBER --pos PARTSOFSPEECH]";

static void checkArgument(bool failureCondition, string_view message) {
    if (failureCondition) {
        cerr << message << endl;
        cerr << USAGE_STRING << endl;
        exit(-1);
    }
}

int main(int argc, const char* const argv[])
{
    inflection::util::ULocale locale("");
    string wordWeightsFilename;
    u16string partsOfSpeech = u"noun,proper-noun";
    int32_t topFrequencies = 32;
    bool verbose = false;

    for (int32_t i = 1; i < argc; i++) {
        if (string("--locale") == argv[i]) {
            checkArgument(!locale.getLanguage().empty(), "Multiple --locale parameters defined");
            checkArgument(i >= argc, "Need a locale after --locale");
            locale = inflection::util::ULocale(argv[++i]);
        } else if (string("--weights") == argv[i]) {
            checkArgument(!wordWeightsFilename.empty(), "Multiple --weights parameters defined");
            checkArgument(i >= argc, "Need a file path after --weights");
            wordWeightsFilename = argv[++i];
        } else if (string("--top") == argv[i]) {
            checkArgument(i >= argc, "--top requires a numeric argument");
            topFrequencies = atoi(argv[++i]);
        } else if (string("--pos") == argv[i]) {
            checkArgument(i >= argc, "--pos requires parts of speech");
            partsOfSpeech = inflection::util::StringViewUtils::to_u16string(argv[++i]);
        } else if (string("--verbose") == argv[i]) {
            verbose = true;
        } else {
            checkArgument(true, string("Unknown argument: ") + argv[i]);
        }
    }
    checkArgument(locale.getLanguage().empty(), "Required parameter --locale is missing");

    map<u16string, int32_t> wordWeights;
    if (!wordWeightsFilename.empty()) {
        ifstream weightsFile(wordWeightsFilename);
        checkArgument(!weightsFile.is_open(), wordWeightsFilename + " not found");
        string line;
        while (!getline(weightsFile, line).eof()) {
            auto tabIdx = line.find('\t');
            if (tabIdx == string::npos || line == "Form\tFrequency") {
                continue;
            }
            string_view word(string_view(line).substr(0, tabIdx));
            wordWeights[inflection::util::StringViewUtils::to_u16string(word)] = stoi(line.substr(tabIdx + 1));
        }
    }

    generateTable(locale, partsOfSpeech, wordWeights, topFrequencies, verbose);
    return 0;
}
