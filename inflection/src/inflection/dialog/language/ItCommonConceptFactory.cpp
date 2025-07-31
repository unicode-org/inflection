/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/ItCommonConceptFactory.hpp>

#include <inflection/dialog/language/ItCommonConceptFactory_ItAndList.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

ItCommonConceptFactory::ItCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
    semanticFeatureIndefiniteArticle = semanticFeatureModel.getFeature(::inflection::grammar::synthesis::ItGrammarSynthesizer::ARTICLE_INDEFINITE);
}

ItCommonConceptFactory::~ItCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* ItCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u" o "));
}

inflection::dialog::SemanticConceptList* ItCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new ItCommonConceptFactory_ItAndList(getSemanticFeatureModel(), concepts);
}

inflection::dialog::SpeakableString* ItCommonConceptFactory::quantify(const inflection::dialog::NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::inflection::dialog::SpeakableString formattedNumber({});
    if (number.longValue() == 1) {
        std::unique_ptr<SpeakableString> indefiniteArticle(
            npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureIndefiniteArticle)));
        if (indefiniteArticle == nullptr) {
            formattedNumber = number.asSpokenWords(u"cardinal-masculine");
        } else {
            formattedNumber = ::inflection::dialog::SpeakableString(indefiniteArticle->getPrint(), indefiniteArticle->getPrint());
        }
    } else {
        formattedNumber = number.asSpokenWords(u"cardinal-masculine");
    }
    return super::quantifyFormatted(number, formattedNumber, semanticConcept);
}

inflection::dialog::SpeakableString ItCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (countType == Plurality::Rule::ONE) {
        return formattedNumber + nounPhrase;
    } else {
        return formattedNumber + inflection::dialog::SpeakableString(u" ") + nounPhrase;
    }
}

} // namespace inflection::dialog::language
