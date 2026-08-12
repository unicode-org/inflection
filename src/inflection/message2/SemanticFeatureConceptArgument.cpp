/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/message2/SemanticFeatureConceptArgument.hpp>

#include <inflection/dialog/SemanticFeatureConceptBase.hpp>

namespace inflection::message2 {

SemanticFeatureConceptArgument::SemanticFeatureConceptArgument(::inflection::dialog::SemanticFeatureConceptBase* featureConcept)
    : featureConcept(featureConcept)
{
}

const ::icu::UnicodeString& SemanticFeatureConceptArgument::tag() const {
    static const ::icu::UnicodeString* tagValue = new ::icu::UnicodeString(TAG);
    return *tagValue;
}

const ::inflection::dialog::SemanticFeatureConceptBase* SemanticFeatureConceptArgument::getConcept() const {
    return featureConcept.get();
}

SemanticFeatureConceptArgument::~SemanticFeatureConceptArgument() = default;

} // namespace inflection::message2
