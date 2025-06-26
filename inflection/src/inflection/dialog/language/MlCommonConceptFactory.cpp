/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */

#include <inflection/dialog/language/MlCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::dialog::language {

MlCommonConceptFactory::MlCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

MlCommonConceptFactory::~MlCommonConceptFactory()
{
}

inflection::dialog::SpeakableString MlCommonConceptFactory::quantifyType(
    const ::inflection::dialog::SpeakableString& formattedNumber,
    const SemanticFeatureConceptBase& semanticConcept,
    bool useDefault,
    Plurality::Rule countType) const
{
    ::std::unique_ptr<::inflection::dialog::SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));

        if (Plurality::Rule::ONE == countType) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), GrammemeConstants::NUMBER_SINGULAR());
        } else {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), GrammemeConstants::NUMBER_PLURAL());
        }

        speakableResult.reset(semanticConceptClone->toSpeakableString());
    }

    if (speakableResult == nullptr) {
        speakableResult.reset(semanticConcept.toSpeakableString());
    }

    return quantifiedJoin(formattedNumber, *npc(speakableResult.get()), {}, countType);
}

} // namespace inflection::dialog::language

