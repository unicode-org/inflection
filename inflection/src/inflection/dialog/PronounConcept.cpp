/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/PronounConcept.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/ustring.h>
#include <unicode/stringoptions.h>
#include <algorithm>
#include <map>
#include <mutex>

namespace inflection::dialog {

using inflection::util::StringViewUtils;
using inflection::util::DelimitedStringIterator;

static constexpr char16_t DEPENDENCY_EQUALS[] = u"dependency=";
static constexpr char16_t SOUND[] = u"sound";
static constexpr char16_t VOWEL_START[] = u"vowel-start";
static constexpr char16_t CONSONANT_START[] = u"consonant-start";
static constexpr char16_t VOWEL_END[] = u"vowel-end";
static constexpr char16_t CONSONANT_END[] = u"consonant-end";

const ::std::map<::std::u16string_view, const char16_t*>& LOCALE_FALLBACK_PAIRS() {
    static auto LOCALE_FALLBACK_PAIRS_ = new ::std::map<::std::u16string_view, const char16_t*>(::inflection::resources::DataResource::getProperties(u"/org/unicode/inflection/inflection/pronoun.properties"));
    return *npc(LOCALE_FALLBACK_PAIRS_);
}

static std::pair<std::u16string, const char16_t *> getPronounTable(const inflection::util::ULocale& locale) {
    const char16_t * reader;
    auto localeItr(locale);
    const auto& localeFallbackPairs(LOCALE_FALLBACK_PAIRS());
    while ((reader = ::inflection::resources::DataResource::getString(u"/org/unicode/inflection/inflection/pronoun_" + localeItr.toString() + u".csv", true)) == nullptr
           && !localeItr.getLanguage().empty())
    {
        // We're falling back so that we can have es_ES and es_MX --> es and zh_TW --> zh_Hant.
        if (const auto fallbackResult = localeFallbackPairs.find(localeItr.toString()); fallbackResult != localeFallbackPairs.end()) {
            localeItr = util::ULocale(util::StringViewUtils::to_string(fallbackResult->second));
        }
        else {
            localeItr = localeItr.getFallback();
        }
    }
    if (reader == nullptr) {
        throw exception::IllegalArgumentException(u"Unsupported language for pronouns: " + locale.toString());
    }
    return {localeItr.toString(), reader};
}

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

struct PronounConcept::DefaultPronounData {
    std::vector<PronounEntry> data {  };
    // Any derived values that doesn't have the data stored elsewhere.
    std::vector<std::unique_ptr<std::u16string>> singletons;

    std::u16string_view getSingleton(std::map<std::u16string_view, std::u16string_view>& allSingletons, std::u16string_view string) {
        auto foundString = allSingletons.find(string);
        if (foundString == allSingletons.end()) {
            auto key = new std::u16string(string);
            std::u16string_view keyView(*npc(key));
            allSingletons.emplace(keyView, keyView);
            return *singletons.emplace_back(key);
        }
        return foundString->second;
    }
};

std::shared_ptr<PronounConcept::DefaultPronounData> PronounConcept::createPronounDataForModel(const SemanticFeatureModel& model, const char16_t* const readerCharArray) {
    std::u16string_view reader(readerCharArray);
    auto result = std::make_shared<PronounConcept::DefaultPronounData>();
    auto &defaultPronounData = result->data;
    PronounConcept::ConstraintData constraints;
    std::map<std::u16string_view, std::u16string_view> singletons;
    defaultPronounData.reserve(std::count(reader.begin(), reader.end(), u'\n'));
    constexpr ::std::u16string_view dependencyPrefix(DEPENDENCY_EQUALS);
    for (DelimitedStringIterator iterator(reader, u"\n"); iterator.hasNext(); ++iterator) {
        ::std::u16string_view word;
        constraints.clear();
        for (DelimitedStringIterator cell(*iterator, u","); cell.hasNext(); ++cell) {
            if (word.empty()) {
                word = *cell;
            } else {
                ::std::u16string_view constraint = *cell;
                singletons.emplace(constraint, constraint);
                auto isDependency = constraint.starts_with(dependencyPrefix);
                if (isDependency) {
                    constraint = constraint.substr(dependencyPrefix.length());
                }
                const SemanticFeature* semanticFeature = nullptr;
                auto [featureAlias, featureValue] = model.getFeatureAlias(constraint);
                if (featureAlias == nullptr) {
                    if (const auto equalsIdx = constraint.find(u'='); equalsIdx != std::u16string::npos) {
                        featureValue = constraint.substr(equalsIdx + 1);
                        constraint = constraint.substr(0, equalsIdx);
                    }
                    // else featureAlias.second will have an empty string to represent all values.
                    semanticFeature = model.getFeature(constraint);
                    if (semanticFeature == nullptr) {
                        throw exception::InvalidConfigurationException(u"Unknown constraint for PronounConcept: " + std::u16string(constraint));
                    }
                } else {
                    semanticFeature = featureAlias;
                }
                std::u16string_view featureValueView = result->getSingleton(singletons, featureValue);
                if (isDependency) {
                    constraints.emplace(result->getSingleton(singletons, std::u16string(dependencyPrefix) + semanticFeature->getName()), featureValueView);
                } else {
                    constraints.emplace(result->getSingleton(singletons, npc(semanticFeature)->getName()), featureValueView);
                }
            }
        }
        defaultPronounData.emplace_back(word, constraints);
    }
    return result;
}

std::map<std::u16string, std::shared_ptr<PronounConcept::DefaultPronounData>>* PronounConcept::PRONOUN_DATA_CACHE() {
    static auto PRONOUN_DATA_CACHE_ = new std::map<std::u16string, std::shared_ptr<PronounConcept::DefaultPronounData>>();
    return npc(PRONOUN_DATA_CACHE_);
}

std::shared_ptr<PronounConcept::DefaultPronounData> PronounConcept::getPronounData(const SemanticFeatureModel& model) {
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    const auto pronounDataCache = PRONOUN_DATA_CACHE();
    const auto [pronounLocale, reader] = getPronounTable(model.getLocale());
    auto pronounDataItr = pronounDataCache->find(pronounLocale);
    if (pronounDataItr == pronounDataCache->end()) {
        pronounDataItr = pronounDataCache->emplace(pronounLocale, createPronounDataForModel(model, reader)).first;
    }
    return pronounDataItr->second;
}

class PronounConcept::PronounData {
    std::shared_ptr<DefaultPronounData> defaultPronounData;
    std::vector<PronounEntry> customizedPronounData {  };
    std::vector<std::unique_ptr<std::u16string>> singletons;

public:
    PronounData(const PronounData&) = delete;
    PronounData& operator=(const PronounData&) = delete;

    PronounData(const SemanticFeatureModel& model, const ::std::vector<DisplayValue>& defaultDisplayData)
        : defaultPronounData(getPronounData(model))
    {
        customizedPronounData.reserve(defaultDisplayData.size());
        std::map<std::u16string_view, std::u16string_view> singletons;
        for (const auto &displayValue: defaultDisplayData) {
            ConstraintData constraintData;
            for (const auto &[semanticFeature, constraint]: displayValue.getConstraintMap()) {
                constraintData.emplace(getSingleton(singletons, semanticFeature.getName()), getSingleton(singletons, constraint));
            }
            customizedPronounData.emplace_back(getSingleton(singletons, displayValue.getDisplayString()), std::move(constraintData));
        }
    }

    const std::vector<PronounEntry>& getCustomizedPronounData() const {
        return customizedPronounData;
    }

    int32_t numValues() const {
        return int32_t(customizedPronounData.size() + defaultPronounData->data.size());
    }

    const PronounEntry& getPronounEntry(int32_t idx) const{
        if (idx < (int32_t)customizedPronounData.size()) {
            return customizedPronounData.at(idx);
        }
        return defaultPronounData->data.at(idx - customizedPronounData.size());
    }

    std::u16string_view getSingleton(std::map<std::u16string_view, std::u16string_view>& allSingletons, std::u16string_view string) {
        auto foundString = allSingletons.find(string);
        if (foundString == allSingletons.end()) {
            auto key = new std::u16string(string);
            std::u16string_view keyView(*npc(key));
            allSingletons.emplace(keyView, keyView);
            return *singletons.emplace_back(key);
        }
        return foundString->second;
    }
};

// Implementation required to be able to look up into map with const SemanticFeature* as keys
std::optional<::std::u16string_view> PronounConcept::getFeatureValueForPronoun(const PronounConcept::ConstraintData &constraintData, const SemanticFeature& feature) {
    auto result = constraintData.find(feature.getName());
    if (result == constraintData.end()) {
        return {};
    }
    return result->second;
}

PronounConcept::PronounConcept(const SemanticFeatureModel& model, const ::std::vector<::inflection::dialog::DisplayValue>& defaultDisplayData, const ::std::map<SemanticFeature, ::std::u16string>& defaultConstraints)
    : super(&model)
    , pronounData(std::make_shared<PronounData>(model, defaultDisplayData))
{
    if (pronounData->numValues() == 0) {
        throw ::inflection::exception::IllegalArgumentException(u"Display data can not be empty.");
    }
    for (const auto &[semanticFeature, grammeme]: defaultConstraints) {
        this->defaultConstraints.emplace(semanticFeature.getName(), grammeme);
    }
}

PronounConcept::PronounConcept(const SemanticFeatureModel& model, const ::std::map<SemanticFeature, ::std::u16string>& defaultConstraints)
    : PronounConcept(model, {}, defaultConstraints)
{
}

PronounConcept::PronounConcept(const SemanticFeatureModel& model, std::u16string_view initialPronoun)
    : PronounConcept(model, {}, {})
{
    int32_t foundPronounIndex = 0;
    for (int32_t idx = 0; idx < pronounData->numValues(); idx++) {
        const auto& pronounEntry = pronounData->getPronounEntry(idx);
        std::u16string_view displayString(pronounEntry.first);
        if (displayString.back() == u' ') {
            displayString.remove_suffix(1);
        }
        auto status = U_ZERO_ERROR;
        if (u_strCaseCompare((const UChar*)initialPronoun.data(), (int32_t)initialPronoun.length(), (const UChar*)displayString.data(), (int32_t)displayString.length(), U_FOLD_CASE_DEFAULT, &status) == 0
            && (bool)U_SUCCESS(status))
        {
            for (const auto& [semanticFeature, constraint] : pronounEntry.second) {
                auto [elementIter, elementInserted] = defaultConstraints.insert({std::u16string(semanticFeature), std::u16string(constraint)});
                if (!elementInserted && elementIter->second != constraint) {
                    // There is some ambiguity. Set the value to an empty string to clear it later.
                    elementIter->second.clear();
                }
            }
            if (defaultPronounIndex < 0) {
                defaultPronounIndex = foundPronounIndex;
            }
        }
        foundPronounIndex++;
    }

    if (defaultPronounIndex < 0) {
        throw ::inflection::exception::IllegalArgumentException(u"Unknown pronoun: " + std::u16string(initialPronoun));
    }

    // Clear the ambiguous constraints
    auto defaultConstraintsEnd = defaultConstraints.end();
    for (auto itr = defaultConstraints.begin(); itr != defaultConstraintsEnd;) {
        if (itr->second.empty()) {
            itr = defaultConstraints.erase(itr);
        }
        else {
            ++itr;
        }
    }
}

PronounConcept::PronounConcept(const PronounConcept& other)
    : super(other)
    , pronounData(other.pronounData)
    , defaultConstraints(other.defaultConstraints)
    , defaultPronounIndex(other.defaultPronounIndex)
{
}

PronounConcept::~PronounConcept()
{
}

bool PronounConcept::isMatchingSoundProperty(const ::std::u16string& displayValue, std::u16string_view matchType) const
{
    if (matchType == CONSONANT_START || matchType == VOWEL_START) {
        auto startsWithVowel = dictionary::PhraseProperties::isStartsWithVowel(npc(getModel())->getLocale(), displayValue);
        return !((startsWithVowel && matchType == CONSONANT_START) || (!startsWithVowel && matchType == VOWEL_START));
    }
    auto endsWithVowel = dictionary::PhraseProperties::isEndsWithVowel(npc(getModel())->getLocale(), displayValue);
    return !((endsWithVowel && matchType == CONSONANT_END) || (!endsWithVowel && matchType == VOWEL_END));
}

bool PronounConcept::isMatchingInitialPronoun() const
{
    if (defaultPronounIndex < 0) {
        return false;
    }
    if (!constraints.empty()) {
        const auto &valueConstraintMap = pronounData->getPronounEntry(defaultPronounIndex).second;
        for (const auto &[semanticName, semanticValue]: constraints) {
            auto valueConstraint = valueConstraintMap.find(semanticName.getName());
            if (valueConstraint == valueConstraintMap.end() || valueConstraint->second != semanticValue) {
                return false;
            }
        }
    }
    return true;
}

static bool isMatchingFeatureValue(const SemanticFeatureConceptBase *referencedConcept, std::u16string_view semanticValue, std::u16string_view semanticNameView)
{
    std::unique_ptr<SpeakableString> featureValue(npc(referencedConcept)->getFeatureValueByName(semanticNameView));
    return !((featureValue == nullptr && semanticValue.empty()) || (featureValue != nullptr && featureValue->getPrint() != semanticValue));
}

PronounConcept::MatchState PronounConcept::getMatchState(const PronounConcept::ConstraintData& valueConstraintMap, const std::u16string& semanticName, const std::u16string& semanticValue)
{
    auto valueConstraint = valueConstraintMap.find(semanticName);
    if (valueConstraint == valueConstraintMap.end()) {
        return NO_MATCH;
    }
    if (valueConstraint->second != semanticValue) {
        if (valueConstraint->second.empty()) {
            // It applies to all values, but perhaps there is another one later that is more specific, like them versus him or her.
            return GENERIC_MATCH;
        }
        return NO_MATCH;
    }
    return FULL_MATCH;
}

const PronounConcept::PronounEntry* PronounConcept::getFirstPossibleValue(const inflection::dialog::SemanticFeatureConceptBase* referencedConcept, bool returnDefault, bool matchAll) const
{
    if (referencedConcept == nullptr && isMatchingInitialPronoun()) {
        return &pronounData->getPronounEntry(defaultPronounIndex);
    }
    const PronounEntry* possibleValue = nullptr;
    int32_t defaultConstraintsToMatch = (int32_t)defaultConstraints.size();
    int32_t genericConstraintsMatched = 0;
    std::unique_ptr<SpeakableString> cachedDisplayValue;
    int32_t numToMatch;
    if (matchAll) {
        numToMatch = pronounData->numValues();
    }
    else {
        numToMatch = (int32_t)pronounData->getCustomizedPronounData().size();
    }
    for (int32_t idx = 0; idx < numToMatch; idx++) {
        const auto &pronounEntry = pronounData->getPronounEntry(idx);
        const auto& [word, valueConstraintMap] = pronounEntry;
        bool fullMatch = true;
        int32_t currGenericConstraintsMatched = 0;
        for (const auto& [semanticName, semanticValue] : constraints) {
            auto matchResult = getMatchState(valueConstraintMap, semanticName.getName(), semanticValue);
            if (matchResult == NO_MATCH) {
                fullMatch = false;
                break;
            } else if (matchResult == GENERIC_MATCH) {
                // It applies to all values, but perhaps there is another one later that is more specific, like them versus him or her.
                currGenericConstraintsMatched++;
            }
            // else FULL_MATCH
        }
        if (fullMatch && referencedConcept != nullptr) {
            for (const auto& [semanticName, semanticValue] : valueConstraintMap) {
                auto equalsIdx = semanticName.find(u'=');
                if (equalsIdx != std::u16string::npos) {
                    std::u16string_view semanticNameView(semanticName);
                    semanticNameView = semanticNameView.substr(equalsIdx + 1);
                    if (semanticNameView == SOUND) {
                        if (!cachedDisplayValue) {
                            cachedDisplayValue.reset(referencedConcept->toSpeakableString());
                        }
                        if (cachedDisplayValue) {
                            fullMatch = isMatchingSoundProperty(cachedDisplayValue->getPrint(), semanticValue);
                        }
                    }
                    else {
                        fullMatch = isMatchingFeatureValue(referencedConcept, semanticValue, semanticNameView);
                    }
                    if (!fullMatch) {
                        break;
                    }
                }
            }
        }
        if (fullMatch) {
            int32_t currDefaultConstraintsToMatch = int32_t(defaultConstraints.size());
            for (const auto& [semanticName, semanticValue] : defaultConstraints) {
                if (getMatchState(valueConstraintMap, semanticName, semanticValue) != NO_MATCH) {
                    currDefaultConstraintsToMatch--;
                }
            }
            if (possibleValue == nullptr || currGenericConstraintsMatched < genericConstraintsMatched || (currGenericConstraintsMatched == genericConstraintsMatched && currDefaultConstraintsToMatch < defaultConstraintsToMatch)) {
                possibleValue = &pronounEntry;
                defaultConstraintsToMatch = currDefaultConstraintsToMatch;
                genericConstraintsMatched = currGenericConstraintsMatched;
                if (genericConstraintsMatched == 0 && defaultConstraintsToMatch == 0) {
                    // We matched all the constraints and the defaults. We can't get any better.
                    break;
                }
                // else we matched all the constraints, but only some default constraints were matched.
                // Let's keep going to see if we can find a better match that matches more defaults.
            }
        }
    }
    if (possibleValue == nullptr && returnDefault) {
        possibleValue = &pronounData->getPronounEntry(0);
    }
    return possibleValue;
}

SpeakableString* PronounConcept::getFeatureValue(const SemanticFeature& feature) const
{
    auto tempCurrentValue = getCurrentValue(nullptr, true, true);
    if (tempCurrentValue != nullptr) {
        auto featureValue = getFeatureValueForPronoun(npc(tempCurrentValue)->second, feature);
        if (featureValue) {
            return new SpeakableString(*featureValue);
        }
    }
    return nullptr;
}

const PronounConcept::PronounEntry* PronounConcept::getCurrentValue(const inflection::dialog::SemanticFeatureConceptBase* referencedConcept, bool returnDefault, bool matchAll) const
{
    const PronounEntry* currentValue = getFirstPossibleValue(referencedConcept, returnDefault, matchAll);
    if (currentValue != nullptr && currentValue->first.empty()) {
        currentValue = nullptr;
    }
    return currentValue;
}

bool PronounConcept::isExists() const
{
    return getCurrentValue(nullptr, false, true) != nullptr;
}

bool PronounConcept::isCustomMatch() const
{
    return getCurrentValue(nullptr, false, false) != nullptr;
}

SpeakableString* PronounConcept::toSpeakableString() const
{
    const auto displayValue = getCurrentValue(nullptr, true, true);
    if (displayValue == nullptr) {
        return nullptr;
    }
    const auto printString(npc(displayValue)->first);
    if (printString.empty()) {
        return nullptr;
    }
    const auto speakString = getFeatureValueForPronoun(npc(displayValue)->second, *npc(getSpeakFeature()));
    if (speakString) {
        return new SpeakableString(printString, *speakString);
    }
    return new SpeakableString(printString);
}

SpeakableString* PronounConcept::toSpeakableString(const inflection::dialog::SemanticFeatureConceptBase& referencedConcept) const
{
    const auto displayValue = getCurrentValue(&referencedConcept, true, true);
    if (displayValue == nullptr) {
        return nullptr;
    }
    const auto printString(npc(displayValue)->first);
    if (printString.empty()) {
        return nullptr;
    }
    const auto speakString = getFeatureValueForPronoun(npc(displayValue)->second, *npc(getSpeakFeature()));
    if (speakString) {
        return new SpeakableString(printString, *speakString);
    }
    return new SpeakableString(printString);
}

::std::u16string PronounConcept::toString() const
{
    ::std::unique_ptr<SpeakableString> string(toSpeakableString());
    return string == nullptr ? ::std::u16string(u"null") : string->toString();
}

PronounConcept* PronounConcept::clone() const
{
    return new PronounConcept(*this);
}

bool PronounConcept::operator==(const PronounConcept& other) const
{
    if (&other == this) {
        return true;
    }
    return getModel()->getLocale() == other.getModel()->getLocale() &&
            pronounData->getCustomizedPronounData() == other.pronounData->getCustomizedPronounData() &&
            defaultConstraints == other.defaultConstraints &&
            defaultPronounIndex == other.defaultPronounIndex &&
            getSpeakFeature() == other.getSpeakFeature() &&
            constraints == other.constraints;
}

} // namespace inflection::dialog
