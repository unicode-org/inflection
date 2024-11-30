/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/ItCommonConceptFactory.hpp>

#include <morphuntion/dialog/language/ItCommonConceptFactory_ItAndList.hpp>
#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

ItCommonConceptFactory::ItCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
    semanticFeatureIndefiniteArticle = semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::ItGrammarSynthesizer::ARTICLE_INDEFINITE);
}

ItCommonConceptFactory::~ItCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* ItCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u" o "));
}

morphuntion::dialog::SemanticConceptList* ItCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new ItCommonConceptFactory_ItAndList(getSemanticFeatureModel(), concepts);
}

morphuntion::dialog::SpeakableString* ItCommonConceptFactory::quantify(const morphuntion::dialog::NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::morphuntion::dialog::SpeakableString formattedNumber({});
    if (number.longValue() == 1) {
        auto indefiniteArticle = npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureIndefiniteArticle));
        if (indefiniteArticle == nullptr) {
            formattedNumber = number.asSpokenWords(u"cardinal-masculine");
        } else {
            formattedNumber = ::morphuntion::dialog::SpeakableString(npc(indefiniteArticle)->getPrint(), npc(indefiniteArticle)->getPrint());
            delete indefiniteArticle;
        }
    } else {
        formattedNumber = number.asSpokenWords(u"cardinal-masculine");
    }
    return super::quantify(number, formattedNumber, semanticConcept);
}

morphuntion::dialog::SpeakableString ItCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (countType == Plurality::Rule::ONE) {
        return formattedNumber + nounPhrase;
    } else {
        return formattedNumber + morphuntion::dialog::SpeakableString(u" ") + nounPhrase;
    }
}

} // namespace morphuntion::dialog::language
