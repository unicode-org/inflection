/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticConcept.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/npc.hpp>
#include <map>
#include <memory>

namespace inflection::dialog {

SemanticConcept::SemanticConcept(const SemanticFeatureModel* model, const SemanticValue& semantic, bool defaultToSemantic)
    : super(model)
    , semantic(semantic)
    , defaultToSemanticValue(semantic.getValue())
    , displayData(npc(model)->getDisplayData(semantic))
    , defaultToSemantic(defaultToSemantic)
{
    if (!defaultToSemantic && (displayData == nullptr || npc(displayData)->getValues().empty())) {
        throw ::inflection::exception::IllegalArgumentException(u"Display data is missing for semantic value \"" + semantic.toString() + u"\" and it can not be displayed by default");
    }
}

SemanticConcept::SemanticConcept(const SemanticConcept& other)
    : super(other)
    , semantic(other.semantic)
    , defaultToSemanticValue(other.defaultToSemanticValue)
    , displayData(other.displayData)
    , defaultToSemantic(other.defaultToSemantic)
{
}

SemanticConcept::~SemanticConcept()
{
}

const ::std::u16string& SemanticConcept::getSemanticValue()
{
    return semantic.getValue();
}

const DisplayValue* SemanticConcept::getFirstPossibleValue(std::unique_ptr<DisplayValue>& generatedDisplayValue) const
{
    const DisplayValue* possibleValue = nullptr;
    if (displayData != nullptr) {
        for (const auto& value : npc(displayData)->getValues()) {
            auto valueConstraintMap = value.getConstraintMap();
            bool fullMatch = true;
            for (const auto& constraint : constraints) {
                auto valueConstraint = valueConstraintMap.find(constraint.first);
                if (valueConstraint == valueConstraintMap.end() || valueConstraint->second != constraint.second) {
                    fullMatch = false;
                }
            }
            if (fullMatch) {
                possibleValue = &value;
                break;
            }
        }
    }
    if (possibleValue == nullptr) {
        auto defaultDisplayFunction = npc(getModel())->getDefaultDisplayFunction();
        if (defaultDisplayFunction != nullptr) {
            DisplayValue* displayValue = nullptr;
            if (displayData == nullptr) {
                if (defaultToSemantic) {
                    SemanticFeatureModel_DisplayData defaultDisplayData({defaultToSemanticValue});
                    displayValue = npc(defaultDisplayFunction)->getDisplayValue(defaultDisplayData, constraints, true);
                }
                else {
                    throw ::inflection::exception::IllegalStateException(u"Display data is missing for \"" + semantic.toString() + u"\" and it can not be displayed by default");
                }
            }
            else {
                displayValue = npc(defaultDisplayFunction)->getDisplayValue(*npc(displayData), constraints, true);
            }
            if (displayValue != nullptr) {
                generatedDisplayValue.reset(displayValue);
                possibleValue = displayValue;
            }
        }
    }
    return possibleValue;
}

SpeakableString* SemanticConcept::getFeatureValue(const SemanticFeature& feature) const
{
    std::unique_ptr<DisplayValue> generatedDisplayValue;
    auto tempCurrentValue = getCurrentValue(generatedDisplayValue);
    if (tempCurrentValue == nullptr) {
        return nullptr;
    }
    auto featureValue = npc(tempCurrentValue)->getFeatureValue(feature);
    if (featureValue != nullptr) {
        return new SpeakableString(*npc(featureValue));
    }
    auto defaultFeatureFunction = npc(getModel())->getDefaultFeatureFunction(feature);
    if (defaultFeatureFunction != nullptr) {
        return npc(defaultFeatureFunction)->getFeatureValue(*npc(tempCurrentValue), constraints);
    }
    auto displayValue = getFirstPossibleValue(generatedDisplayValue);
    if (displayValue != nullptr) {
        auto synonymFeatureValue = npc(displayValue)->getFeatureValue(feature);
        if (synonymFeatureValue != nullptr) {
            return new SpeakableString(*npc(synonymFeatureValue));
        }
    }
    return nullptr;
}

const DisplayValue* SemanticConcept::getCurrentValue(std::unique_ptr<DisplayValue>& generatedDisplayValue) const
{
    const DisplayValue* currentValue;
    auto currentPossibleValue = getFirstPossibleValue(generatedDisplayValue);
    if (currentPossibleValue != nullptr && !npc(currentPossibleValue)->getDisplayString().empty()) {
        currentValue = currentPossibleValue;
    } else if (defaultToSemantic && !defaultToSemanticValue.getDisplayString().empty()) {
        currentValue = &defaultToSemanticValue;
    } else {
        currentValue = nullptr;
    }
    return currentValue;
}

bool SemanticConcept::isExists() const
{
    std::unique_ptr<DisplayValue> generatedDisplayValue;
    return getCurrentValue(generatedDisplayValue) != nullptr;
}

SpeakableString* SemanticConcept::toSpeakableString() const
{
    return toSpeakableStringDefault(defaultToSemantic);
}

SpeakableString* SemanticConcept::toSpeakableStringDefault(bool defaultToSemantic) const
{
    ::std::u16string printString;
    std::unique_ptr<DisplayValue> generatedDisplayValue;
    auto displayValue = getCurrentValue(generatedDisplayValue);
    if (displayValue == nullptr) {
        if (defaultToSemantic) {
            printString = semantic.getValue();
        }
        else {
            return nullptr;
        }
    }
    else {
        printString = npc(displayValue)->getDisplayString();
    }
    if (printString.empty()) {
        return nullptr;
    }
    if (displayValue != nullptr) {
        auto speakString = npc(displayValue)->getFeatureValue(*npc(getSpeakFeature()));
        if (speakString != nullptr) {
            return new SpeakableString(printString, *npc(speakString));
        }
    }
    return new SpeakableString(printString);
}

::std::u16string SemanticConcept::toString() const
{
    return toString(defaultToSemantic);
}

::std::u16string SemanticConcept::toString(bool defaultToSemantic) const
{
    ::std::unique_ptr<SpeakableString> string(toSpeakableStringDefault(defaultToSemantic));
    return string == nullptr ? ::std::u16string(u"null") : string->toString();
}

SemanticConcept* SemanticConcept::clone() const
{
    return new SemanticConcept(*this);
}

bool SemanticConcept::operator==(const SemanticConcept& other) const
{
    if (&other == this) {
        return true;
    }
    return getModel() == other.getModel() && semantic == other.semantic && defaultToSemantic == other.defaultToSemantic && getSpeakFeature() == other.getSpeakFeature() && constraints == other.constraints;
}

} // namespace inflection::dialog
