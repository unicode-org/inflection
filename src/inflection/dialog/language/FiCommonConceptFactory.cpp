/*
 * Copyright 2019-2026 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/FiCommonConceptFactory.hpp>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <memory>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::dialog::language {

FiCommonConceptFactory::FiCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::CASE, {
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ABLATIVE, u"cardinal-ablative"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ADESSIVE, u"cardinal-adessive"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ALLATIVE, u"cardinal-allative"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ELATIVE, u"cardinal-elative"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ESSIVE, u"cardinal-essive"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_GENITIVE, u"cardinal-genitive"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_ILLATIVE, u"cardinal-illative"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_INESSIVE, u"cardinal-inessive"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE, u"cardinal"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_PARTITIVE, u"cardinal-partitive"},
        {::inflection::grammar::synthesis::GrammemeConstants::CASE_TRANSLATIVE, u"cardinal-translative"}
    })
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::FINNISH())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::FINNISH()))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&singularProperty, {GrammemeConstants::NUMBER_SINGULAR}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&pluralProperty, {GrammemeConstants::NUMBER_PLURAL}));
    ::inflection::util::Validate::notNull(semanticFeatureForNumberConcept);
}

inflection::dialog::SpeakableString FiCommonConceptFactory::quantifyType(const inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool /*useDefault*/, Plurality::Rule countType) const
{
    ::std::unique_ptr<inflection::dialog::SpeakableString> speakableFeatureValue(semanticConcept.getFeatureValue(*npc(semanticFeatureForNumberConcept)));
    ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
    bool shouldUsePluralVar = shouldUsePlural(semanticConcept);
    if (shouldUsePluralVar) {
        semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL);
    } else {
        semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR);
    }

    if ((speakableFeatureValue == nullptr || speakableFeatureValue->getPrint() == ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE)
        && (!shouldUsePluralVar && Plurality::Rule::ONE != countType))
    {
        semanticConceptClone->putConstraint(*npc(semanticFeatureForNumberConcept), ::inflection::grammar::synthesis::GrammemeConstants::CASE_PARTITIVE);
    }
    ::std::unique_ptr<inflection::dialog::SpeakableString> speakableResult(npc(semanticConceptClone->toSpeakableString()));
    return formattedNumber + inflection::dialog::SpeakableString(u" ") + *speakableResult;
}

inflection::dialog::SpeakableString* FiCommonConceptFactory::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::std::unique_ptr<inflection::dialog::SpeakableString> speakableFeatureValue(npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureForNumberConcept)));
    std::u16string baseRuleName(u"cardinal");
    if (speakableFeatureValue != nullptr) {
        auto rulename = semanticFeatureToNumberConceptName.find(speakableFeatureValue->getPrint());
        if (rulename != semanticFeatureToNumberConceptName.end()) {
            baseRuleName = rulename->second;
        }
    }
    if (shouldUsePlural(*npc(semanticConcept))) {
        baseRuleName += u"-plural";
    }
    auto formattedNumber(number.asSpokenWords(baseRuleName));
    return super::quantifyFormatted(number, formattedNumber, semanticConcept);
}

bool FiCommonConceptFactory::isPluraleTantumPattern(const ::inflection::dictionary::Inflector_InflectionPattern& pattern) const {
    return !pattern.containsGrammemes(singularProperty);
}

bool FiCommonConceptFactory::isPluraleTantum(std::u16string_view word) const {
    std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    inflector.getInflectionPatternsForWord(word, inflectionPatterns);
    for (const auto& pattern : inflectionPatterns) {
        if (isPluraleTantumPattern(pattern)) {
            return true;
        }
    }
    return false;
}

bool FiCommonConceptFactory::shouldUsePlural(const inflection::dialog::SemanticFeatureConceptBase& semanticConcept) const
{
    bool shouldUsePlural = false;
    if (semanticConcept.hasConstraint(*npc(semanticFeatureCount))) {
        shouldUsePlural = *npc(semanticConcept.getConstraint(*npc(semanticFeatureCount))) == ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL;
    }

    if (!shouldUsePlural) {
        ::std::unique_ptr<SemanticFeatureConceptBase> withoutConstraints(npc(semanticConcept.clone()));
        withoutConstraints->reset();
        ::std::unique_ptr<inflection::dialog::SpeakableString> speakableString(npc(withoutConstraints->toSpeakableString()));
        if (speakableString != nullptr) {
            const std::u16string& headWord = speakableString->getPrint();
            if (!headWord.empty()) {
                shouldUsePlural = isPluraleTantum(headWord) || dictionary.hasAllProperties(headWord, pluralProperty);
            }
        }
    }
    return shouldUsePlural;
}

} // namespace inflection::dialog::language
