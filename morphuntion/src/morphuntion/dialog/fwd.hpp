/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for morphuntion.dialog
#pragma once

#include <morphuntion/api.h>
// MORPHUNTION_CLASS_API

namespace morphuntion
{
    //! Dialog composition. Includes classes for semantic composition and word inflection.
    namespace dialog
    {
        /// @cond FORWARD_DECLARATIONS
        class ArticleDetectionFunction;
        class CommonConceptFactory;
        class CommonConceptFactoryImpl;
        class CommonConceptFactoryProvider;
        class DefaultArticleLookupFunction;
        class DefaultDisplayFunction;
        class DefaultFeatureFunction;
        class DefinitenessDisplayFunction;
        class DeterminerAdpositionDetectionFunction;
        class DictionaryLookupFunction;
        class DictionaryLookupInflector;
        class Plurality;
        class InflectableStringConcept;
        class LocalizedCommonConceptFactoryProvider;
        class NumberConcept;
        class PronounConcept;
        class SemanticConcept;
        class SemanticConceptList;
        class SemanticFeature;
        class SemanticFeatureConceptBase;
        class SemanticFeatureModel;
        class SemanticFeatureModel_DisplayData;
        class SemanticFeatureModel_DisplayValue;
        class SemanticUtils;
        class SemanticValue;
        class SpeakableConcept;
        class SpeakableString;
        class StaticArticleLookupFunction;
        /// @endcond
    } // dialog
} // morphuntion
