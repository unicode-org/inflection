/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlInflectionPattern::NlGrammarSynthesizer_NlInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DUTCH())))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    this->declensionFeature = model.getFeature(NlGrammarSynthesizer::DECLENSION());
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->caseFeature = model.getFeature(GrammemeConstants::CASE);
}

bool NlGrammarSynthesizer_NlInflectionPattern::isAdjective(const ::std::u16string& word) const
{
    if (!dictionary.isKnownWord(word)) {
        return false;
    }
    int64_t binaryType = 0;
    return (dictionary.getCombinedBinaryType(&binaryType, word) != nullptr && (binaryType & dictionaryAdjective) != 0);
}

bool NlGrammarSynthesizer_NlInflectionPattern::isNoun(const ::std::u16string& word) const
{
    if (!dictionary.isKnownWord(word)) {
        return false;
    }
    int64_t binaryType = 0;
    return (dictionary.getCombinedBinaryType(&binaryType, word) != nullptr && (binaryType & dictionaryNoun) != 0);
}

void NlGrammarSynthesizer_NlInflectionPattern::logUnsupportedInflection(const ::std::u16string& displayString, const ::std::u16string& reason)
{
    if (::inflection::util::LoggerConfig::isDebugEnabled()) {
        ::inflection::util::Logger::debug(u"Unsupported inflection of '" + displayString + u"': " + reason);
    }
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidDeclensionConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto declension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
    return NlGrammarSynthesizer::getDeclension(&declension) != NlGrammarSynthesizer::Declension::undefined;
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidCaseConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto wordCase(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    return NlGrammarSynthesizer::getCase(&wordCase) != NlGrammarSynthesizer::Case::undefined;
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidCountConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto count(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    return NlGrammarSynthesizer::getCount(&count) != NlGrammarSynthesizer::Count::undefined;
}

} // namespace inflection::grammar::synthesis
