/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/message2/fwd.hpp>
#include <inflection/dialog/fwd.hpp>

#include <unicode/messageformat2_formattable.h>
#include <unicode/unistr.h>

#include <memory>

/**
 * @brief A FormattableObject wrapper that carries an inflection SemanticFeatureConceptBase
 * (a SemanticConcept or PronounConcept) through MessageFormat 2 as an argument of type
 * UFMT_OBJECT, tagged "SemanticFeatureConcept".
 *
 * The SemanticFeatureModel the concept references is owned elsewhere (it is expensive to
 * build) and must outlive this wrapper. Use ClassCastException::verifyCast on getConcept()
 * to recover the concrete concept subclass.
 */
class INFLECTION_CLASS_API inflection::message2::SemanticFeatureConceptArgument
    : public ::icu::message2::FormattableObject
{
public:
    /** The FormattableObject::tag() value identifying this wrapper. */
    static constexpr char16_t TAG[] = u"SemanticFeatureConcept";

    /** Takes ownership of the concept (a SemanticConcept or PronounConcept). */
    explicit SemanticFeatureConceptArgument(::inflection::dialog::SemanticFeatureConceptBase* featureConcept);
    /** Returns TAG. */
    const ::icu::UnicodeString& tag() const override;
    /** The carried concept; recover the concrete subclass with ClassCastException::verifyCast. */
    const ::inflection::dialog::SemanticFeatureConceptBase* getConcept() const;
    ~SemanticFeatureConceptArgument() override;

private:
    // The concept caches a pointer into a SemanticFeatureModel that is owned elsewhere.
    std::unique_ptr<::inflection::dialog::SemanticFeatureConceptBase> featureConcept;
};
