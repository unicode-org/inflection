/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlVerbInflectionPattern.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/Validate.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlVerbInflectionPattern::NlGrammarSynthesizer_NlVerbInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::DUTCH()))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DUTCH())))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryVerb, {GrammemeConstants::POS_VERB()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
}

NlGrammarSynthesizer_NlVerbInflectionPattern::~NlGrammarSynthesizer_NlVerbInflectionPattern()
{
}

::std::u16string NlGrammarSynthesizer_NlVerbInflectionPattern::inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto requestedPOS(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));
    auto targetCount = getGrammaticalNumber(constraints);
    if (targetCount == NlGrammarSynthesizer::Number::undefined
        || (wordGrammemes & dictionaryVerb) == 0
        || (!requestedPOS.empty() && requestedPOS != GrammemeConstants::POS_VERB()))
    {
        // Can't inflect
        return {};
    }
    if (constraints.size() == 1
        && ((NlGrammarSynthesizer::Number::singular == targetCount && (wordGrammemes & dictionarySingular) == dictionarySingular)
        || (NlGrammarSynthesizer::Number::plural == targetCount && (wordGrammemes & dictionaryPlural) == dictionaryPlural)))
    {
        // The requested inflection is already the provided form.
        return displayString;
    }
    std::vector<dictionary::Inflector_InflectionPattern> patterns;
    inflector.getInflectionPatternsForWord(displayString, patterns);
    if (!patterns.empty()) {
        ::std::u16string inflection;
        int64_t toConstraintGrammemes;
        if (targetCount == NlGrammarSynthesizer::Number::singular) {
            toConstraintGrammemes = dictionarySingular;
        }
        else {
            toConstraintGrammemes = dictionaryPlural;
        }
        for (const auto& pattern : patterns) {
            if ((pattern.getPartsOfSpeech() & dictionaryVerb) == 0) {
                continue;
            }
            inflection = pattern.reinflect(wordGrammemes, toConstraintGrammemes, displayString);
            if (!inflection.empty()) {
                return inflection;
            }
            // Failed to inflect.
        }
        // inflection will be empty upon a failure.
    }
    return {};
}

NlGrammarSynthesizer::Number
NlGrammarSynthesizer_NlVerbInflectionPattern::getGrammaticalNumber(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    return NlGrammarSynthesizer::getNumber(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
}

} // namespace inflection::grammar::synthesis
