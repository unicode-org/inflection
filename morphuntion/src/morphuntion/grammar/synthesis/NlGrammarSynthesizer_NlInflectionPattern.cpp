/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_NlInflectionPattern::NlGrammarSynthesizer_NlInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::DUTCH())))
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
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
    if (::morphuntion::util::LoggerConfig::isDebugEnabled()) {
        ::morphuntion::util::Logger::debug(u"Unsupported inflection of '" + displayString + u"': " + reason);
    }
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidDeclensionConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto declension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
    return NlGrammarSynthesizer::getDeclension(&declension) != NlGrammarSynthesizer::Declension::undefined;
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidCaseConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto wordCase(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    return NlGrammarSynthesizer::getCase(&wordCase) != NlGrammarSynthesizer::Case::undefined;
}

bool NlGrammarSynthesizer_NlInflectionPattern::isValidCountConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (constraints.empty()) {
        return false;
    }
    auto count(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
    return NlGrammarSynthesizer::getCount(&count) != NlGrammarSynthesizer::Count::undefined;
}

} // namespace morphuntion::grammar::synthesis
