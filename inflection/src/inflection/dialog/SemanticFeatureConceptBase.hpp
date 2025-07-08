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
    /// @cond
    typedef SpeakableConcept super;
    /// @endcond

private: /* package */
    const SemanticFeatureModel* model {  };
protected: /* package */
    /**
     * All the constraints
     */
    ::std::map<SemanticFeature, ::std::u16string> constraints {  };

public: /* protected */
    /**
     * Returns the SemanticFeatureModel used to create this string.
     */
    virtual const SemanticFeatureModel* getModel() const;
    /**
     * Get the value of the semantic feature.
     * @param feature The semantic feature object.
     * @return The value of the constraint feature.
     */
    virtual const ::std::u16string* getConstraint(const SemanticFeature& feature) const;
    /**
     * Check if the semantic feature has been added as a constraint.
     * @param feature The semantic feature object.
     * @return True if the semantic feature is in this, false otherwise.
     */
    virtual bool hasConstraint(const SemanticFeature& feature) const;
    /**
     * Add a semantic feature with a value as a constraint.
     * @param feature The semantic feature object of the constraint to be added.
     * @param featureValue The value of the feature.
     */
    virtual void putConstraint(const SemanticFeature& feature, ::std::u16string_view featureValue);
    /**
     * Add a semantic feature with a value as a constraint by the name of the SemanticFeature.
     * @param featureName The name of the feature.
     * @param featureValue The value of the feature.
     */ 
    virtual void putConstraintByName(::std::u16string_view featureName, ::std::u16string_view featureValue);
    /**
     * Clear the specified semantic constraint feature.
     * @param feature The semantic feature of the constraint to be cleared.
     */    
    virtual void clearConstraint(const SemanticFeature& feature);
    /**
     * Clear the specified semantic constraint feature by the given feature name.
     * @param featureName The name of the constraint to be cleared.
     */ 
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
     * Returns the String value of the specified feature for this concept, given its 
     * current constraints.
     * The featureName will be automatically converted to a SemanticFeature for lookup.
     */
    virtual SpeakableString* getFeatureValueByName(::std::u16string_view featureName) const;
protected: /* protected */
    /**
     * The semantic feature for the spoken form of a string.
     */
    virtual const SemanticFeature* getSpeakFeature() const;

public:
    /**
     * Return a clone of this object with a deep copy of the constraints.
     */
    virtual SemanticFeatureConceptBase* clone() const = 0;

    /**
     * This constructor for the abstract class should be called by the subclasses.
     * @param model The SemanticFeatureModel that provides all of the features, 
     *              valid constraints, default functions
     *              and other relevant information for a given language.
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
