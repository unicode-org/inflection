/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SpeakableConcept.hpp>
#include <map>

/**
 * @brief Abstract base class of SpeakableConcept with inflectable constraints.
 * @details
 * This abstract base class allows a SemanticFeatureConcept to be constrained with the possible semantic features and
 * format to a value to display/speak. A SemanticFeature could be grammatical gender, grammatical number, grammatical
 * number or other relevant semantic feature.
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureConceptBase
    : public virtual SpeakableConcept
{
public:
    /**
     * super class.
     */
    typedef SpeakableConcept super;

private: /* package */
    const SemanticFeatureModel* model {  };
protected: /* package */
    ::std::map<SemanticFeature, ::std::u16string> constraints {  };

public: /* protected */
    /**
     * Returns the SemanticFeatureModel used to create this string.
     */
    virtual const SemanticFeatureModel* getModel() const;
    virtual const ::std::u16string* getConstraint(const SemanticFeature& feature) const;
    virtual bool hasConstraint(const SemanticFeature& feature) const;
    virtual void putConstraint(const SemanticFeature& feature, ::std::u16string_view featureValue);
    virtual void putConstraintByName(::std::u16string_view featureName, ::std::u16string_view featureValue);
    virtual void clearConstraint(const SemanticFeature& feature);
    virtual void clearConstraintByName(::std::u16string_view featureName);
    /**
     * Returns {@code true} if a value exists for the given constraints
     */
    virtual bool isExists() const = 0;
    /**
     * Clear all constraints imposed on this SemanticFeatureConcept.
     */
    virtual void reset();
    /**
     * Returns the String value of the specified feature for this concept, given its current constraints
     */
    virtual SpeakableString* getFeatureValue(const SemanticFeature& feature) const = 0;
    /**
     * Returns the String value of the specified feature for this concept, given its current constraints.
     * The featureName will be automatically converted to a SemanticFeature for lookup.
     */
    virtual SpeakableString* getFeatureValueByName(::std::u16string_view featureName) const;
protected: /* protected */
    virtual const SemanticFeature* getSpeakFeature() const;

public:
    /**
     * Return a clone of this object with a deep copy of the constraints.
     */
    virtual SemanticFeatureConceptBase* clone() const = 0;

    /**
     * This constructor for the abstract class should be called by the subclasses.
     * @param model The SemanticFeatureModel that provides all of the features, valid constraints, default functions
     * and other relevant information for a given language.
     */
    explicit SemanticFeatureConceptBase(const SemanticFeatureModel* model);
    /**
     * Copy constructor
     */
    SemanticFeatureConceptBase(const SemanticFeatureConceptBase& other);
    /**
     * Destructor
     */
    ~SemanticFeatureConceptBase() override;
private:
    SemanticFeatureConceptBase& operator=(const SemanticFeatureConceptBase&) = delete;
};
