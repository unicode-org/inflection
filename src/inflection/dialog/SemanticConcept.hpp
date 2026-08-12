/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticValue.hpp>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Provides a way to format a semantic value with specific display values for a given language.
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticConcept
    : public SemanticFeatureConceptBase
{
public:
    /// @cond
    typedef SemanticFeatureConceptBase super;
    /// @endcond

private:
    SemanticValue semantic;
    DisplayValue defaultToSemanticValue;
    const SemanticFeatureModel_DisplayData *displayData {  };
    bool defaultToSemantic {  };

public:
    /**
     * Return the semantic value used to create this object. This value is used when defaulting to the semantic.
     * @see SemanticValue#getValue
     */
    virtual const ::std::u16string& getSemanticValue();

private: /* protected */
    const DisplayValue* getFirstPossibleValue(std::unique_ptr<DisplayValue>& generatedDisplayValue) const;
    const DisplayValue* getCurrentValue(std::unique_ptr<DisplayValue>& generatedDisplayValue) const;

public:
    /**
     * Returns the requested semantic feature. For example, you can request the definite article semantic feature in some languages.
     */
    ::inflection::dialog::SpeakableString* getFeatureValue(const SemanticFeature& feature) const override;

    /**
     * Returns true if a value exists for the given constraints
     */
    bool isExists() const override;

    /**
     * Returns the requested value for the provided constraints.
     * @return null when the constrained values is an empty set.
     */
    ::inflection::dialog::SpeakableString* toSpeakableString() const override;

    /**
     * Returns the requested value for the provided constraints.
     * @param defaultToSemantic When this is true and the constrained values is an empty set, use the semantic value instead.
     */
    virtual ::inflection::dialog::SpeakableString* toSpeakableStringDefault(bool defaultToSemantic) const;

    /**
     * Return a debuggable formatted string of this SemanticConcept.
     */
    ::std::u16string toString() const override;

private:
    ::std::u16string toString(bool defaultToSemantic) const;
public:
    /**
     * Create a clone of this object. This is helpful if you need to preserve the constraints of the current object.
     */
    SemanticConcept* clone() const override;

public:
    /**
     * Returns true when both objects refer to the same model, have the same semantic value, have the same constraints and the same defaultToSemantic value.
     */
    bool operator==(const SemanticConcept& o) const;

    /**
     * Constructs a new SemanticConcept. A SemanticFeatureModel is required, as it will enforce the locale and store display data.
     */
    SemanticConcept(const SemanticFeatureModel* model, const SemanticValue& semantic, bool defaultToSemantic = false);

    /**
     * The copy constructor
     */
    SemanticConcept(const SemanticConcept& other);

    /**
     * The destructor
     */
    ~SemanticConcept() override;
private:
    SemanticConcept& operator=(const SemanticConcept&) = delete;
};
