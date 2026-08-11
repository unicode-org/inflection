/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_FiDisplayFunction.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_LocativeBasedLookupFunction.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_WithHyphenSuffixLookupFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/Normalizer2.hpp>

namespace inflection::grammar::synthesis {

void FiGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto displayFunction = new FiGrammarSynthesizer_FiDisplayFunction(featureModel);
    featureModel.setDefaultDisplayFunction(displayFunction);
    featureModel.putDefaultFeatureFunctionByName(RESIDING, new FiGrammarSynthesizer_LocativeBasedLookupFunction(*npc(displayFunction), FiGrammarSynthesizer::Case::inessive, FiGrammarSynthesizer::Case::adessive));
    featureModel.putDefaultFeatureFunctionByName(EXITING, new FiGrammarSynthesizer_LocativeBasedLookupFunction(*npc(displayFunction), FiGrammarSynthesizer::Case::elative, FiGrammarSynthesizer::Case::ablative));
    featureModel.putDefaultFeatureFunctionByName(ENTERING, new FiGrammarSynthesizer_LocativeBasedLookupFunction(*npc(displayFunction), FiGrammarSynthesizer::Case::illative, FiGrammarSynthesizer::Case::allative));
    featureModel.putDefaultFeatureFunctionByName(WITH_HYPHEN_SUFFIX, new FiGrammarSynthesizer_WithHyphenSuffixLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new FiGrammarSynthesizer_CountLookupFunction(*npc(displayFunction)));
}

::std::vector<::std::vector<::std::vector<::std::u16string>>> FiGrammarSynthesizer::getPossibleCombinations(const ::std::vector<::std::u16string>& arr)
{
    ::std::vector<::std::vector<::std::vector<::std::u16string>>> results({{arr}});
    if (arr.size() == 1) {
        return results;
    }
    auto arrSize = int32_t(arr.size());
    for (int32_t i = 1; i < arrSize; i++) {
        ::std::vector<::std::u16string> head(arr.cbegin(), arr.cbegin() + i);
        ::std::vector<::std::u16string> tail(arr.cbegin() + i, arr.cend());
        auto tails = getPossibleCombinations(tail);
        for (auto& strings : tails) {
            strings.insert(strings.begin(), head);
        }
        results.insert(results.end(), tails.begin(), tails.end());
    }
    return results;
}

bool FiGrammarSynthesizer::hasBackVowels(const ::std::u16string& lemma)
{
    static auto NFKC = new ::icu4cxx::Normalizer2(::icu4cxx::Normalizer2::getNFKCInstance());
    constexpr std::u16string_view BACK_VOWELS = u"aou";

    // We must use composed characters, because if we use decomposed characters, front-vowels "ö" and "ä" will match
    // back-vowels "o" and "a" because the decomposite forms are "o¨", "a¨" and thus would create a false positives
    bool foundBackVowel = false;
    for (auto ch : inflection::dictionary::PhraseProperties::normalize(*npc(NFKC), lemma)) {
        if (foundBackVowel) {
            if (u_isbase(ch)) {
                return true;
            }
            foundBackVowel = false;
        }
        ch = static_cast<char16_t>(u_tolower(ch));
        if (BACK_VOWELS.find(ch) != std::u16string_view::npos) {
            foundBackVowel = true;
        }
    }
    return foundBackVowel;
}

FiGrammarSynthesizer::Number FiGrammarSynthesizer::getCount(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

::std::u16string FiGrammarSynthesizer::toString(FiGrammarSynthesizer::Number value) {
    if (value == Number::singular) {
        return GrammemeConstants::NUMBER_SINGULAR;
    }
    if (value == Number::plural) {
        return GrammemeConstants::NUMBER_PLURAL;
    }
    return u"";
}

FiGrammarSynthesizer::Case FiGrammarSynthesizer::getCase(const ::std::u16string* value) {
    static constexpr struct {
        const char16_t* key;
        Case value;
    } valueMap[] = {
        {GrammemeConstants::CASE_ABLATIVE, Case::ablative},
        {GrammemeConstants::CASE_ADESSIVE, Case::adessive},
        {GrammemeConstants::CASE_ALLATIVE, Case::allative},
        {GrammemeConstants::CASE_ELATIVE, Case::elative},
        {GrammemeConstants::CASE_ESSIVE, Case::essive},
        {GrammemeConstants::CASE_GENITIVE, Case::genitive},
        {GrammemeConstants::CASE_ILLATIVE, Case::illative},
        {GrammemeConstants::CASE_INESSIVE, Case::inessive},
        {GrammemeConstants::CASE_NOMINATIVE, Case::nominative},
        {GrammemeConstants::CASE_PARTITIVE, Case::partitive},
        {GrammemeConstants::CASE_TRANSLATIVE, Case::translative},
    };
    if (value != nullptr) {
        auto *entry = inflection::util::ArrayUtils::searchSorted<valueMap>(*npc(value),
            [](const auto& item) { return item.key; });
        if (entry != nullptr) {
            return entry->value;
        }
    }
    return Case::undefined;
}

::std::u16string FiGrammarSynthesizer::toString(FiGrammarSynthesizer::Case value) {
    static constexpr struct {
        Case key;
        const char16_t* value;
    } valueMap[] = {
        {Case::nominative, GrammemeConstants::CASE_NOMINATIVE},
        {Case::genitive, GrammemeConstants::CASE_GENITIVE},
        {Case::partitive, GrammemeConstants::CASE_PARTITIVE},
        {Case::inessive, GrammemeConstants::CASE_INESSIVE},
        {Case::elative, GrammemeConstants::CASE_ELATIVE},
        {Case::illative, GrammemeConstants::CASE_ILLATIVE},
        {Case::adessive, GrammemeConstants::CASE_ADESSIVE},
        {Case::ablative, GrammemeConstants::CASE_ABLATIVE},
        {Case::allative, GrammemeConstants::CASE_ALLATIVE},
        {Case::essive, GrammemeConstants::CASE_ESSIVE},
        {Case::translative, GrammemeConstants::CASE_TRANSLATIVE},
    };
    auto *entry = inflection::util::ArrayUtils::searchSorted<valueMap>(value,
        [](const auto& item) { return item.key; });
    if (entry != nullptr) {
        return entry->value;
    }
    return u"";
}

FiGrammarSynthesizer::Person FiGrammarSynthesizer::getPronounPerson(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::PERSON_FIRST) {
            return Person::first;
        }
        if (valRef == GrammemeConstants::PERSON_SECOND) {
            return Person::second;
        }
        if (valRef == GrammemeConstants::PERSON_THIRD) {
            return Person::third;
        }
    }
    return Person::undefined;
}

} // namespace inflection::grammar::synthesis
