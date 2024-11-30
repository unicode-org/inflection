/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>

#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog {

SemanticFeatureConceptBase::SemanticFeatureConceptBase(const SemanticFeatureModel* model)
    : super()
    , model(model)
{
}

SemanticFeatureConceptBase::SemanticFeatureConceptBase(const SemanticFeatureConceptBase& other)
    : super(other)
    , model(other.model)
    , constraints(other.constraints)
{
}

SemanticFeatureConceptBase::~SemanticFeatureConceptBase()
{
}

const SemanticFeatureModel* SemanticFeatureConceptBase::getModel() const
{
    return model;
}

const ::std::u16string* SemanticFeatureConceptBase::getConstraint(const SemanticFeature& feature) const
{
    auto result = constraints.find(feature);
    if (result == constraints.end()) {
        return nullptr;
    }
    return &result->second;
}

bool SemanticFeatureConceptBase::hasConstraint(const SemanticFeature& feature) const
{
    auto result = constraints.find(feature);
    return result != constraints.end();
}

void SemanticFeatureConceptBase::putConstraint(const SemanticFeature& feature, const ::std::u16string& featureValue)
{
    constraints[feature] = featureValue;
}

void SemanticFeatureConceptBase::putConstraintByName(const ::std::u16string& featureName, const ::std::u16string& featureValue)
{
    auto feature = npc(getModel())->getFeature(featureName);
    if (feature == nullptr) {
        throw ::morphuntion::exception::IllegalArgumentException(featureName + u" is not a valid feature name for the SemanticFeatureModel");
    }
    putConstraint(*npc(feature), featureValue);
}

void SemanticFeatureConceptBase::clearConstraint(const SemanticFeature& feature)
{
    constraints.erase(feature);
}

void SemanticFeatureConceptBase::clearConstraintByName(const ::std::u16string& featureName)
{
    auto feature = npc(getModel())->getFeature(featureName);
    if (feature == nullptr) {
        throw ::morphuntion::exception::IllegalArgumentException(featureName + u" is not a valid feature name for the SemanticFeatureModel");
    }
    clearConstraint(*npc(feature));
}

void SemanticFeatureConceptBase::reset()
{
    constraints.clear();
}

const SemanticFeature* SemanticFeatureConceptBase::getSpeakFeature() const
{
    return model != nullptr ? npc(model)->getFeature(SemanticFeatureModel::SPEAK) : static_cast<const SemanticFeature* >(nullptr);
}

SpeakableString* SemanticFeatureConceptBase::getFeatureValueByName(const ::std::u16string& featureName) const
{
    auto const feature = npc(getModel())->getFeature(featureName);
    if (feature == nullptr) {
        throw ::morphuntion::exception::IllegalArgumentException(featureName + u" is not a valid feature name for the SemanticFeatureModel");
    }
    return getFeatureValue(*npc(feature));
}


} // namespace morphuntion::dialog
