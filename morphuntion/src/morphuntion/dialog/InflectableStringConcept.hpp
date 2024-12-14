/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <optional>
#include <string>

/**
 * @brief Provides a way to format a word with additional grammatical category values or semantic features of a word for a given language.
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::InflectableStringConcept
    : public SemanticFeatureConceptBase
{
public:
    typedef SemanticFeatureConceptBase super;

private:
    SpeakableString value;
    SemanticFeatureModel_DisplayValue defaultDisplayValue;

public: /* protected */
    /**
     * Returns the Speakable String representing the given semantic feature.
     *
     * @param feature - the semantic feature for which the value is requested.
     * @return Speakable String
     */
    SpeakableString* getFeatureValue(const SemanticFeature& feature) const override;

public:
    /**
     *
     * @return true when the value is empty.
     */
    bool isExists() const override;
    ::std::u16string toString() const override;

private:
    ::std::optional<SemanticFeatureModel_DisplayValue> getDisplayValue(bool allowInflectionGuess = true) const;

public:
    /**
     * Converts the concept to a Speakable String
     *
     * @return Speakable String
     */
    SpeakableString* toSpeakableString() const override;
    /**
     * Clones the concept
     *
     * @return concept
     */
    InflectableStringConcept* clone() const override;

    /**
     * Constructs a concept given a semantic feature model and a speakable string
     *
     * @param model - The semantic feature model required to initialize the concept.
     * @param value - The speakable string to convert to a concept
     */
    InflectableStringConcept(const SemanticFeatureModel* model, const SpeakableString& value);
    /**
     * Copy constructor
     */
    InflectableStringConcept(const InflectableStringConcept& other);
    /**
     * Destructor
     */
    ~InflectableStringConcept() override;
private:
    InflectableStringConcept& operator=(const InflectableStringConcept&) = delete;
};