/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Provides a way to format a customizable pronoun for a given language.
 * @details
 * Formatting a pronoun in various languages requires a lot of context. It may be known that the pronoun needs to be in
 * the third person, and that the gender of the pronoun in the message, but the context in the sentence may affect the
 * type of pronoun used or even be affected by the grammatical properties of a separate noun that the pronoun is possessing.
 * For example, in Spanish, the phrase "your message" is "tu mensaje" where "your" is "tu" in Spanish. If there are multiple
 * messages, then the phrase would be "tus mensajes". If you have a scenario where the pronoun depends on the properties
 * of another word, then it's recommended that you use toSpeakableString(const SemanticFeatureConceptBase&) to provide
 * that additional context beyond the constraints.
 */
class INFLECTION_CLASS_API inflection::dialog::PronounConcept
    : public SemanticFeatureConceptBase
{

public:
    /// @cond
    typedef SemanticFeatureConceptBase super;
    /// @endcond

private:
    enum MatchState {
        NO_MATCH,
        GENERIC_MATCH,
        FULL_MATCH
    };
    template<class T> struct ptr_less {
        bool operator()(const T* lhs, const T* rhs) const {
            return *lhs < *rhs;
        }
    };
    typedef std::map<const SemanticFeature*, std::u16string, ptr_less<SemanticFeature>> ConstraintData;
    typedef std::pair<std::u16string, ConstraintData> PronounEntry;
    struct DefaultPronounData;
    class PronounData;

    std::shared_ptr<PronounData> pronounData;
    ::std::map<SemanticFeature, ::std::u16string> defaultConstraints {  };
    int32_t defaultPronounIndex { -1 };

private: /* protected */
    static std::shared_ptr<PronounConcept::DefaultPronounData> createPronounDataForModel(const SemanticFeatureModel& model, const char16_t* const readerCharArray);
    static std::shared_ptr<PronounConcept::DefaultPronounData> getPronounData(const SemanticFeatureModel& model);
    static const ::std::u16string* getFeatureValueForPronoun(const PronounConcept::ConstraintData &constraintData, const SemanticFeature& feature);
    bool isMatchingSoundProperty(const ::std::u16string& displayValue, std::u16string_view matchType) const;
    bool isMatchingInitialPronoun() const;
    static MatchState getMatchState(const PronounConcept::ConstraintData& valueConstraintMap, const SemanticFeature& semanticName, const std::u16string& semanticValue);
    const PronounEntry* getFirstPossibleValue(const inflection::dialog::SemanticFeatureConceptBase* referencedConcept, bool returnDefault, bool matchAll) const;
    const PronounEntry* getCurrentValue(const inflection::dialog::SemanticFeatureConceptBase* referencedConcept, bool returnDefault, bool matchAll) const;

public:
    /**
     * Returns the requested semantic feature. For example, you can request the gender semantic feature in some languages.
     */
    ::inflection::dialog::SpeakableString* getFeatureValue(const SemanticFeature& feature) const override;

    /**
     * Returns true if a value exists for the given constraints
     */
    bool isExists() const override;

    /**
     * Returns true if the given constraints match a custom pronoun specified in the constructor.
     */
    bool isCustomMatch() const;

    /**
     * Returns the requested value for the provided constraints.
     * @return null when the constrained values is an empty set.
     */
    ::inflection::dialog::SpeakableString* toSpeakableString() const override;

    /**
     * Returns the requested value for the provided constraints in addition
     * to the constraints and semantic feature values of the referenced concept.
     * It's important to note that some pronouns are derived
     * from both person being referenced and the object being referenced.
     * @param referencedConcept This concept provides additional semantic feature values based of the referenced concept.
     * @return null when the constrained values is an empty set.
     */
    virtual ::inflection::dialog::SpeakableString* toSpeakableString(const inflection::dialog::SemanticFeatureConceptBase& referencedConcept) const;

    /**
     * Return a debuggable formatted string of this PronounConcept.
     */
    ::std::u16string toString() const override;

    /**
     * Create a clone of this object. This is helpful if you need to preserve the constraints of the current object.
     */
    PronounConcept* clone() const override;

    /**
     * Returns true when both objects refer to the same model, have the same semantic value, have the same constraints and the same defaultToSemantic value.
     */
    bool operator==(const PronounConcept& o) const;

    /**
     * Constructs a new PronounConcept based on custom pronouns. Any unspecified pronouns will use the language's default pronouns.
     * @param model The SemanticFeatureModel for the language
     * @param defaultDisplayData These are customized pronouns that override the default pronouns.
     * @param defaultConstraints The constraints to consider when a given constraint is undefined.
     */
    PronounConcept(const SemanticFeatureModel& model, const ::std::vector<::inflection::dialog::DisplayValue>& defaultDisplayData, const ::std::map<SemanticFeature, ::std::u16string>& defaultConstraints);

    /**
     * Constructs a new PronounConcept with the default constraints. If you know the gender or person grammatical category
     * values of the desired pronoun, it's recommended to provide them with defaultConstraints.
     * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
     * @param model The SemanticFeatureModel for the language
     * @param defaultConstraints The constraints to consider when a given constraint is undefined.
     */
    PronounConcept(const SemanticFeatureModel& model, const ::std::map<SemanticFeature, ::std::u16string>& defaultConstraints);

    /**
     * Constructs a new PronounConcept with the default constraints based on an existing pronoun from the language.
     * For example, "I" will provide the default constraints "first", "singular" and "subjective" for English.
     * If "her" was provided for initialPronoun in English, then the default constraints will be "third", "singular", "feminine", and "objective".
     * If "her" was the in English, and then the constraints "possessive" and "independent" were added, then formatting
     * this PronounConcept with toSpeakableString() will provide "hers". The "possessive" constraint would override
     * the "objective" constraint.
     *
     * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
     * @param model The SemanticFeatureModel for the language
     * @param initialPronoun The language specific pronoun to start inflecting the pronoun.
     * @throws IllegalArgumentException Thrown when the pronoun is not a known pronoun for the given language.
     */
    PronounConcept(const SemanticFeatureModel& model, std::u16string_view initialPronoun);

    /**
     * The copy constructor
     */
    PronounConcept(const PronounConcept& other);

    /**
     * The destructor
     */
    ~PronounConcept() override;
private:
    static std::map<std::u16string, std::shared_ptr<PronounConcept::DefaultPronounData>>* PRONOUN_DATA_CACHE();

    PronounConcept& operator=(const PronounConcept&) = delete;
};
