/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/CsGrammarSynthesizer.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/PrefixedDisplayFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>

namespace inflection::grammar::synthesis {

std::vector<std::tuple<std::u16string_view, int64_t, int64_t>> CsGrammarSynthesizer::getPrefixesWithPos(const ::inflection::dictionary::DictionaryMetaData& dictionary)
{
    std::vector<std::tuple<std::u16string_view, int64_t, int64_t>> prefixesWithPOS;
    int64_t dictionaryAdjective = 0;

    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE}));
    prefixesWithPOS.emplace_back(u"nej", dictionaryAdjective, 0);
    return prefixesWithPOS;
}

void CsGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::CZECH(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::CZECH(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::CZECH(), {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_VOCATIVE}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::ANIMACY, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::CZECH(), {GrammemeConstants::ANIMACY_ANIMATE, GrammemeConstants::ANIMACY_INANIMATE, GrammemeConstants::ANIMACY_HUMAN}));
    featureModel.setDefaultDisplayFunction(new ::inflection::grammar::PrefixedDisplayFunction(featureModel, ::inflection::util::LocaleUtils::CZECH(), {
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE},
        {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_VOCATIVE},
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER},
        {GrammemeConstants::ANIMACY_INANIMATE, GrammemeConstants::ANIMACY_ANIMATE},
    }, GrammemeConstants::ANIMACY, &CsGrammarSynthesizer::getPrefixesWithPos));
}

} // namespace inflection::grammar::synthesis
