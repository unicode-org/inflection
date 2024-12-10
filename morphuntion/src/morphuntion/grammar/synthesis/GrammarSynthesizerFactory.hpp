/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <set>

/**
 * @brief Factory for creating Morphuntion's GrammarSynthesizers.
 * @details This class can be used to instantiate a GrammarSynthesizer directly, or get a list of locales we currently
 * support for grammar synthesis using getKnownLocales().
 *
 * For grammar synthesis, it is recommend to use either morphuntion::dialog::SemanticFeatureModel or
 * morphuntion::dialog::CommonConceptFactory.
 */
class MORPHUNTION_INTERNAL_API morphuntion::grammar::synthesis::GrammarSynthesizerFactory final
{
public:
    /**
     * Adds all the semantic features required for the SemanticFeatureModel with a GrammarSynthesizer.
     *
     * @param locale The @ref morphuntion::util::ULocale "ULocale" to request the semantic features.
     * @param featureModel The @ref morphuntion::dialog::SemanticFeatureModel "featureModel" to get a GrammarSynthesizer in.
     */
    static void addSemanticFeatures(const ::morphuntion::util::ULocale& locale, ::morphuntion::dialog::SemanticFeatureModel& featureModel);
    /**
     * Returns a set of locales Morphuntion supports for grammar synthesis.
     */
    static const ::std::set<::morphuntion::util::ULocale>& getKnownLocales();

private:
    GrammarSynthesizerFactory() = delete;
};
