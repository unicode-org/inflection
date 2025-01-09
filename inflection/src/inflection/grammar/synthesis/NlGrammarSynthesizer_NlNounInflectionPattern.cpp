/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/Validate.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlNounInflectionPattern::NlGrammarSynthesizer_NlNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::DUTCH()))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isNoun(displayString)) {
        logUnsupportedInflection(displayString, u"not a noun or not in dictionary");
        return false;
    }
    if (constraints.empty() || GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature).empty()) {
        return false;
    }
    return true;
}

::std::u16string NlGrammarSynthesizer_NlNounInflectionPattern::inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isSupported(displayString, constraints)) {
        return {};
    }
    auto targetCount(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    ::std::u16string inflection;
    if (constraints.size() == 1
        && ((GrammemeConstants::NUMBER_SINGULAR() == targetCount && dictionary.hasAllProperties(displayString, dictionarySingular))
        || (GrammemeConstants::NUMBER_PLURAL() == targetCount && dictionary.hasAllProperties(displayString, dictionaryPlural))))
    {
        // The requested inflection is already the provided form.
        return displayString;
    }
    auto inflectionType = dictionary.getPropertyValues(displayString, u"inflection");
    if (!inflectionType.empty()) {
        const auto& patternName = inflectionType.front();
        auto inflectionPattern = inflector.getInflectionPatternByName(patternName);
        if (!inflectionPattern) {
            ::inflection::util::Logger::warn(u"Lemma " + displayString + u" has inflection=" + patternName + u" but that pattern was not found");
        } else {
            int64_t wordGrammemes = 0;
            dictionary.getCombinedBinaryType(&wordGrammemes, displayString);
            int64_t toConstraintGrammemes = 0;
            dictionary.getBinaryProperties(&toConstraintGrammemes, {targetCount});
            inflection = inflectionPattern->reinflect(wordGrammemes, toConstraintGrammemes, displayString);
            // inflection will be empty upon a failure.
        }
    }
    return inflection;
}

} // namespace inflection::grammar::synthesis
