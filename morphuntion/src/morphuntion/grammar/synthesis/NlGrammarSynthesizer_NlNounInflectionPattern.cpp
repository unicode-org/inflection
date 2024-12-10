/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/Validate.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_NlNounInflectionPattern::NlGrammarSynthesizer_NlNounInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super(model)
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::DUTCH()))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
}

bool NlGrammarSynthesizer_NlNounInflectionPattern::isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
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

::std::u16string NlGrammarSynthesizer_NlNounInflectionPattern::inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
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
            ::morphuntion::util::Logger::warn(u"Lemma " + displayString + u" has inflection=" + patternName + u" but that pattern was not found");
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

} // namespace morphuntion::grammar::synthesis
