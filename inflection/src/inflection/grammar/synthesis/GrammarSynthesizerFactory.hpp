/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <set>

/**
 * @brief Factory for creating Inflection's GrammarSynthesizers.
 * @details This class can be used to instantiate a GrammarSynthesizer directly, or get a list of locales we currently
 * support for grammar synthesis using getKnownLocales().
 *
 * For grammar synthesis, it is recommend to use either inflection::dialog::SemanticFeatureModel or
 * inflection::dialog::CommonConceptFactory.
 */
class INFLECTION_INTERNAL_API inflection::grammar::synthesis::GrammarSynthesizerFactory final
{
public:
    /**
     * Adds all the semantic features required for the SemanticFeatureModel with a GrammarSynthesizer.
     *
     * @param locale The @ref inflection::util::ULocale "ULocale" to request the semantic features.
     * @param featureModel The @ref inflection::dialog::SemanticFeatureModel "featureModel" to get a GrammarSynthesizer in.
     */
    static void addSemanticFeatures(const ::inflection::util::ULocale& locale, ::inflection::dialog::SemanticFeatureModel& featureModel);
    /**
     * Returns a set of locales Inflection supports for grammar synthesis.
     */
    static const ::std::set<::inflection::util::ULocale>& getKnownLocales();

private:
    GrammarSynthesizerFactory() = delete;
};
