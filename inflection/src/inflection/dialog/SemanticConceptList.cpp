/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticConceptList.hpp>

#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::dialog {

SemanticConceptList::SemanticConceptList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model)
{
    for (auto conceptObj : values) {
        if (conceptObj != nullptr) {
            concepts.push_back(npc(conceptObj)->clone());
        }
    }
}

static ::std::vector<const SemanticFeatureConceptBase*> convertToConst(const ::std::vector<SemanticFeatureConceptBase*>& values)
{
    ::std::vector<const SemanticFeatureConceptBase*> results;
    results.reserve(values.size());
    for (auto value : values) {
        if (value != nullptr) {
            results.push_back(value);
        }
    }
    return results;
}

SemanticConceptList::SemanticConceptList(const SemanticFeatureModel* model, const ::std::vector<SemanticFeatureConceptBase*>& values)
    : SemanticConceptList::SemanticConceptList(model, convertToConst(values))
{
}

SemanticConceptList::SemanticConceptList(const SemanticConceptList& other)
    : super(other)
    , beforeFirst(other.beforeFirst)
    , afterFirst(other.afterFirst)
    , itemDelimiter(other.itemDelimiter)
    , beforeLast(other.beforeLast)
    , afterLast(other.afterLast)
    , itemPrefix(other.itemPrefix)
    , itemSuffix(other.itemSuffix)
{
    for (auto conceptObj : other.concepts) {
        if (conceptObj != nullptr) {
            concepts.push_back(npc(conceptObj)->clone());
        }
    }
}

SemanticConceptList::~SemanticConceptList()
{
    for (auto conceptObj : concepts) {
        delete conceptObj;
    }
}

void SemanticConceptList::putConstraint(const SemanticFeature& feature, ::std::u16string_view featureValue)
{
    for (auto conceptObj : concepts) {
        npc(conceptObj)->putConstraint(feature, featureValue);
    }
    super::putConstraint(feature, featureValue);
}

void SemanticConceptList::clearConstraint(const SemanticFeature& feature)
{
    for (auto conceptObj : concepts) {
        npc(conceptObj)->clearConstraint(feature);
    }
    super::clearConstraint(feature);
}

void SemanticConceptList::reset()
{
    for (auto conceptObj : concepts) {
        npc(conceptObj)->reset();
    }
    super::reset();
}

SpeakableString* SemanticConceptList::getFeatureValue(const SemanticFeature& feature) const
{
    if (!concepts.empty()) {
        if (lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(npc(getModel())->getLocale()).getCategories().count(feature.getName()) == 0) {
            return toSpeakableString(&feature);
        }
        return npc(concepts[0])->getFeatureValue(feature);
    }
    return nullptr;
}

int32_t SemanticConceptList::getSize() const
{
    return int32_t(concepts.size());
}

const SemanticFeatureConceptBase* SemanticConceptList::get(int32_t index) const
{
    return concepts.at(index);
}

const SemanticFeatureConceptBase* SemanticConceptList::operator[](int32_t index) const
{
    return concepts.at(index);
}

bool SemanticConceptList::isExists() const
{
    return !concepts.empty();
}

std::u16string SemanticConceptList::toString() const
{
    SpeakableString displayValue(u"");
    SpeakableString comma(u", ");
    const SemanticFeatureConceptBase* previousConcept = nullptr;
    for (auto conceptObj : concepts) {
        if (previousConcept != nullptr) {
            displayValue += comma;
        }
        ::std::unique_ptr<::inflection::dialog::SpeakableString> conceptResult(npc(conceptObj)->toSpeakableString());
        if (conceptResult.get() != nullptr) {
            displayValue += *conceptResult;
        }
        previousConcept = conceptObj;
    }
    return u"[" + displayValue.getPrint() + u"]";
}

SpeakableString* SemanticConceptList::toSpeakableString() const
{
    return toSpeakableString(nullptr);
}

SpeakableString* SemanticConceptList::toSpeakableString(const SemanticFeature* feature) const
{
    if (concepts.empty()) {
        return nullptr;
    }
    SpeakableString displayValue(u"");
    SpeakableString empty(u"");
    SpeakableString currentItemDelimiter(u"");
    SpeakableString currentItemPrefix(u"");
    SpeakableString currentItemSuffix(u"");
    ::std::unique_ptr<SpeakableString> previousFormattedResult;

    const SemanticFeatureConceptBase* previousConcept = nullptr;
    const SemanticFeatureConceptBase* lastConcept = concepts.back();
    const SemanticFeatureConceptBase* firstConcept = concepts.front();
    for (auto conceptPtr : concepts) {
        const auto& conceptRef = *npc(conceptPtr);
        SpeakableString* conceptResultPtr = nullptr;
        if (feature != nullptr) {
            conceptResultPtr = conceptRef.getFeatureValue(*npc(feature));
        }
        else {
            conceptResultPtr = conceptRef.toSpeakableString();
        }
        if (conceptResultPtr == nullptr || npc(conceptResultPtr)->isEmpty()) {
            // This shouldn't happen.
            continue;
        }
        const auto& conceptResult = *npc(conceptResultPtr);
        currentItemDelimiter = empty;
        if (firstConcept == conceptPtr) {
            if (beforeFirst.isEmpty()) {
                displayValue += getBeforeFirst(conceptRef, conceptResult);
            }
            else {
                displayValue += beforeFirst;
            }
        }

        if (firstConcept == previousConcept) {
            if (afterFirst.isEmpty()) {
                currentItemDelimiter += getAfterFirst(*npc(previousConcept), *previousFormattedResult, conceptRef, conceptResult);
            }
            else {
                currentItemDelimiter += afterFirst;
            }
        }
        if (lastConcept == conceptPtr && previousConcept != nullptr) {
            if (beforeLast.isEmpty()) {
                currentItemDelimiter += getBeforeLast(*npc(previousConcept), *previousFormattedResult, conceptRef, conceptResult);
            }
            else {
                currentItemDelimiter += beforeLast;
            }
        }
        if (lastConcept != firstConcept && firstConcept != conceptPtr && previousConcept != nullptr) {
            // There is at least one item
            if (currentItemDelimiter.isEmpty()) {
                if (itemDelimiter.isEmpty()) {
                    displayValue += getItemDelimiter(*npc(previousConcept), *previousFormattedResult, conceptRef, conceptResult);
                }
                else {
                    displayValue += itemDelimiter;
                }
            }
            else {
                displayValue += currentItemDelimiter;
            }
        }

        if (itemPrefix.isEmpty()) {
            currentItemPrefix = getItemPrefix(conceptRef, conceptResult);
        }
        else {
            currentItemPrefix = itemPrefix;
        }
        if (itemSuffix.isEmpty()) {
            currentItemSuffix = getItemSuffix(conceptRef, conceptResult);
        }
        else {
            currentItemSuffix = itemSuffix;
        }

        if (!avoidAffixRedundancy || !conceptResult.getPrint().starts_with(currentItemPrefix.getPrint())) {
            displayValue += currentItemPrefix;
        }
        displayValue += conceptResult;
        if (!avoidAffixRedundancy || !conceptResult.getPrint().ends_with(currentItemSuffix.getPrint())) {
            displayValue += currentItemSuffix;
        }

        if (lastConcept == &conceptRef) {
            if (afterLast.isEmpty()) {
                displayValue += getAfterLast(conceptRef, conceptResult);
            }
            else {
                displayValue += afterLast;
            }
        }

        previousConcept = &conceptRef;
        previousFormattedResult.reset(conceptResultPtr);
    }
    return new SpeakableString(displayValue);
}

SemanticConceptList* SemanticConceptList::clone() const
{
    return new SemanticConceptList(*this);
}

SemanticConceptList* SemanticConceptList::setBeforeFirst(const SpeakableString& value)
{
    beforeFirst = value;
    return this;
}

SpeakableString SemanticConceptList::getBeforeFirst(const SemanticFeatureConceptBase& /*first*/, const SpeakableString& /*formattedFirst*/) const
{
    return beforeFirst;
}

SemanticConceptList* SemanticConceptList::setAfterFirst(const SpeakableString& value)
{
    afterFirst = value;
    return this;
}

SpeakableString SemanticConceptList::getAfterFirst(const SemanticFeatureConceptBase& /*first*/, const SpeakableString& /*formattedFirst*/, const SemanticFeatureConceptBase& /*second*/, const SpeakableString& /*formattedSecond*/) const
{
    return afterFirst;
}

SemanticConceptList* SemanticConceptList::setItemDelimiter(const SpeakableString& value)
{
    itemDelimiter = value;
    return this;
}

SpeakableString SemanticConceptList::getItemDelimiter(const SemanticFeatureConceptBase& /*previous*/  , const SpeakableString& /*formattedPrevious*/, const SemanticFeatureConceptBase& /*next*/, const SpeakableString& /*formattedNext*/) const
{
    return itemDelimiter;
}

SemanticConceptList* SemanticConceptList::setBeforeLast(const SpeakableString& value)
{
    beforeLast = value;
    return this;
}

SpeakableString SemanticConceptList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& /*formattedSecondToLast*/, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& /*formattedLast*/) const
{
    return beforeLast;
}

SemanticConceptList* SemanticConceptList::setAfterLast(const SpeakableString& value)
{
    afterLast = value;
    return this;
}

SpeakableString SemanticConceptList::getAfterLast(const SemanticFeatureConceptBase& /*last*/, const SpeakableString& /*formattedLast*/) const
{
    return afterLast;
}

SemanticConceptList* SemanticConceptList::setItemPrefix(const SpeakableString& value)
{
    itemPrefix = value;
    return this;
}

SpeakableString SemanticConceptList::getItemPrefix(const SemanticFeatureConceptBase& /*item*/, const SpeakableString& /*formattedItem*/) const
{
    return itemPrefix;
}

SemanticConceptList* SemanticConceptList::setItemSuffix(const SpeakableString& value)
{
    itemSuffix = value;
    return this;
}

SpeakableString SemanticConceptList::getItemSuffix(const SemanticFeatureConceptBase& /*item*/, const SpeakableString& /*formattedItem*/) const
{
    return itemSuffix;
}

void SemanticConceptList::setAvoidItemAffixRedundancy(bool value)
{
    avoidAffixRedundancy = value;
}

} // namespace inflection::dialog
