/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <string>
#include <vector>

/**
 * @brief Formats a list of SemanticFeatureConceptBase objects with context sensitive semantic features.
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticConceptList
    : public SemanticFeatureConceptBase
{
public:
    /// @cond
    typedef SemanticFeatureConceptBase super;
    /// @endcond

private:
    ::std::vector<SemanticFeatureConceptBase*> concepts {  };
    SpeakableString beforeFirst { {} };
    SpeakableString afterFirst { {} };
    SpeakableString itemDelimiter { {} };
    SpeakableString beforeLast { {} };
    SpeakableString afterLast { {} };
    SpeakableString itemPrefix { {} };
    SpeakableString itemSuffix { {} };
    bool avoidAffixRedundancy { true };

public:
    /**
     * Put constraint feature and value into the SemanticConceptList.
     * @param feature the feature of the constraint to be added.
     * @param featureValue the value of the constraint.
     */
    void putConstraint(const SemanticFeature& feature, ::std::u16string_view featureValue) override;
    /**
     * Clean the constraint feature in the SemanticConceptList.
     * @param feature the feature of the constraint to be clearn.
     */    
    void clearConstraint(const SemanticFeature& feature) override;    
    void reset() override;

public: /* protected */
    SpeakableString* getFeatureValue(const SemanticFeature& feature) const override;

public:
    /**
     * Returns the number of items in the list.
     */
    int32_t getSize() const;
    /**
     * Retrieves the SemanticFeatureConceptBase object at the requested index.
     * @return returns the SemanticFeatureConceptBase object at the requested index.
     * @throw An exception if the index is out of range.
     */
    const SemanticFeatureConceptBase* get(int32_t index) const;
    /**
     * Retrieves the SemanticFeatureConceptBase object at the requested index.
     * @return returns the SemanticFeatureConceptBase object at the requested index.
     * @throw An exception if the index is out of range.
     */
    const SemanticFeatureConceptBase* operator[](int32_t index) const;
    /**
     * Returns true if this list is not empty.
     */
    bool isExists() const override;
    /**
     * Returns a debuggable representation of this list without the separators.
     */
    ::std::u16string toString() const override;

public:
    SpeakableString* toSpeakableString() const override;
private:
    SpeakableString* toSpeakableString(const SemanticFeature* feature) const;
public:
    SemanticConceptList* clone() const override;

    /**
     * If there is at least 1 item, show this value before the entire list.
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setBeforeFirst(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param first The first item in the list.
     * @param formattedFirst The formatted form of the first item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     */
    virtual SpeakableString getBeforeFirst(const SemanticFeatureConceptBase& first, const SpeakableString& formattedFirst) const;

    /**
     * If there are at least 2 items, show this value after the first value and before the second value
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setAfterFirst(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param first The first item in the list.
     * @param formattedFirst The formatted form of the first item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     * @param second The second item in the list.
     * @param formattedSecond The formatted form of the second item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     */
    virtual SpeakableString getAfterFirst(const SemanticFeatureConceptBase& first, const SpeakableString& formattedFirst, const SemanticFeatureConceptBase& second, const SpeakableString& formattedSecond) const;

    /**
     * The default delimiter between items. Usually it is some form of a comma.
     *
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setItemDelimiter(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param previous The previous item in the list.
     * @param formattedPrevious The formatted form of the previous item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     * @param next The mext item in the list.
     * @param formattedNext The formatted form of the next item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     */
    virtual SpeakableString getItemDelimiter(const SemanticFeatureConceptBase& previous, const SpeakableString& formattedPrevious, const SemanticFeatureConceptBase& next, const SpeakableString& formattedNext) const;

    /**
     * If there are at least 2 items, show this value after the second to last value and before the last value
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setBeforeLast(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param secondToLast The second to last item. It is before the last item.
     * @param formattedSecondToLast The formatted form of the second to last item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     * @param last The last item in the list.
     * @param formattedLast The formatted form of the last item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     */
    virtual SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const;

    /**
     * If there is at least 1 item, show this value after the entire list.
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setAfterLast(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param last The last item in the list.
     * @param formattedLast The formatted form of the last item.
     * This should already be formatted with the constraints, item prefix and item suffix as specified in this list.
     */
    virtual SpeakableString getAfterLast(const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const;

    /**
     * For each item, add the provided prefix.
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setItemPrefix(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param item The current item to add a prefix.
     * @param formattedItem The formatted form of the item.
     * This should already be formatted with the constraints as specified in this list.
     */
    virtual SpeakableString getItemPrefix(const SemanticFeatureConceptBase& item, const SpeakableString& formattedItem) const;

    /**
     * For each item, add the provided suffix.
     *
     * @return A pointer to this object to allow chaining.
     */
    virtual SemanticConceptList* setItemSuffix(const SpeakableString& value);

    /**
     * Without the context of the current list, return the SpeakableString given the parameters.
     * @param item The current item to add a suffix.
     * @param formattedItem The formatted form of the item.
     * This should already be formatted with the constraints as specified in this list.
     */
    virtual SpeakableString getItemSuffix(const SemanticFeatureConceptBase& item, const SpeakableString& formattedItem) const;

    /**
     * If the item's prefix or suffix is already a prefix or suffix to an item, should we avoid adding the same affix.
     * The default is true.
     */
    virtual void setAvoidItemAffixRedundancy(bool value);

public:
    /**
     * Creates a list of immutable SemanticConcept
     */
    SemanticConceptList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    /**
     * Creates a list of mutable SemanticConcept
     */
    SemanticConceptList(const SemanticFeatureModel* model, const ::std::vector<SemanticFeatureConceptBase*>& values);
    /**
     * The copy constructor
     */
    SemanticConceptList(const SemanticConceptList& other);
    /**
     * The destructor
     */
    ~SemanticConceptList() override;
private:
    SemanticConceptList& operator=(const SemanticConceptList&) = delete;
};
