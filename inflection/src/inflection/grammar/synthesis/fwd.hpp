/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for inflection.grammar.synthesis
#pragma once

#include <inflection/api.h>

namespace inflection
{
    //! Contains classes related to grammar synthesization.
    namespace grammar
    {
        //! Inflection's grammar synthesizers.
        namespace synthesis
        {
            class ArGrammarSynthesizer;
            class ArGrammarSynthesizer_ArDisplayFunction;
            class ArGrammarSynthesizer_ArticleLookupFunction;
            class ArGrammarSynthesizer_AsPrepositionLookupFunction;
            class ArGrammarSynthesizer_DefinitenessLookupFunction;
            class ArGrammarSynthesizer_PronounLookupFunction;
            class ArGrammarSynthesizer_ToPrepositionLookupFunction;
            class ArGrammarSynthesizer_WithPrepositionLookupFunction;
            class DaGrammarSynthesizer;
            class DaGrammarSynthesizer_ArticleLookupFunction;
            class DaGrammarSynthesizer_DaDisplayFunction;
            class DeGrammarSynthesizer;
            class DeGrammarSynthesizer_ArticleLookupFunction;
            class DeGrammarSynthesizer_DeDisplayFunction;
            class EnGrammarSynthesizer;
            class EnGrammarSynthesizer_ArticleLookupFunction;
            class EnGrammarSynthesizer_CaseLookupFunction;
            class EnGrammarSynthesizer_CountLookupFunction;
            class EnGrammarSynthesizer_EnDisplayFunction;
            class EsGrammarSynthesizer;
            class EsGrammarSynthesizer_ArticleLookupFunction;
            class EsGrammarSynthesizer_CountGenderLookupFunction;
            class EsGrammarSynthesizer_EsDisplayFunction;
            class FrGrammarSynthesizer;
            class FrGrammarSynthesizer_ArticleElidedFunction;
            class FrGrammarSynthesizer_ArticleLookupFunction;
            class FrGrammarSynthesizer_CountLookupFunction;
            class FrGrammarSynthesizer_FrDisplayFunction;
            class FrGrammarSynthesizer_GenderLookupFunction;
            class GrammarSynthesizerFactory;
            class GrammarSynthesizerUtil;
            class GrammemeConstants;
            class HeGrammarSynthesizer;
            class HeGrammarSynthesizer_DefinitenessLookupFunction;
            class HeGrammarSynthesizer_GenderLookupFunction;
            class HeGrammarSynthesizer_HeDisplayFunction;
            class HeGrammarSynthesizer_WithConditionalHyphen;
            class HiGrammarSynthesizer;
            class HiGrammarSynthesizer_GenderLookupFunction;
            class HiGrammarSynthesizer_CountLookupFunction;
            class HiGrammarSynthesizer_HiDisplayFunction;
            class ItGrammarSynthesizer;
            class ItGrammarSynthesizer_ArticleLookupFunction;
            class ItGrammarSynthesizer_CountLookupFunction;
            class ItGrammarSynthesizer_DefiniteArticleLookupFunction;
            class ItGrammarSynthesizer_GenderLookupFunction;
            class ItGrammarSynthesizer_IndefiniteArticleLookupFunction;
            class ItGrammarSynthesizer_ItDisplayFunction;
            class KoGrammarSynthesizer;
            class KoGrammarSynthesizer_KoDisplayFunction;
            class KoGrammarSynthesizer_ParticleLookupFunction;
            class KoGrammarSynthesizer_ParticleResolver;
            class NbGrammarSynthesizer;
            class NbGrammarSynthesizer_ArticleLookupFunction;
            class NbGrammarSynthesizer_NbDisplayFunction;
            class MlGrammarSynthesizer;
            class MlGrammarSynthesizer_NumberLookupFunction;
            class MlGrammarSynthesizer_GenderLookupFunction;
            class MlGrammarSynthesizer_CaseLookupFunction;
            class MlGrammarSynthesizer_MlDisplayFunction;
            class NlGrammarSynthesizer;
            class NlGrammarSynthesizer_ArticleLookupFunction;
            class NlGrammarSynthesizer_DefinitenessLookupFunction;
            class NlGrammarSynthesizer_NlAdjectiveInflectionPattern;
            class NlGrammarSynthesizer_NlDisplayFunction;
            class NlGrammarSynthesizer_NlNounInflectionPattern;
            class NlGrammarSynthesizer_NlVerbInflectionPattern;
            class PtGrammarSynthesizer;
            class PtGrammarSynthesizer_ArticleLookupFunction;
            class PtGrammarSynthesizer_GenericLookupFunction;
            class PtGrammarSynthesizer_PtDisplayFunction;
            class RuGrammarSynthesizer;
            class RuGrammarSynthesizer_AboutPrepositionLookupFunction;
            class RuGrammarSynthesizer_ArticleLookupFunction;
            class RuGrammarSynthesizer_InPrepositionLookupFunction;
            class RuGrammarSynthesizer_RuDisplayFunction;
            class RuGrammarSynthesizer_ToPrepositionLookupFunction;
            class RuGrammarSynthesizer_WithPrepositionLookupFunction;
            class SignificantTokenInflector;
            class SrGrammarSynthesizer;
            class SrGrammarSynthesizer_SrDisplayFunction;
            class SvGrammarSynthesizer;
            class SvGrammarSynthesizer_ArticleLookupFunction;
            class SvGrammarSynthesizer_GenderLookupFunction;
            class SvGrammarSynthesizer_SvDisplayFunction;
            class TrGrammarSynthesizer;
            class TrGrammarSynthesizer_DeConjunctionArticleLookupFunction;
            class TrGrammarSynthesizer_InterrogativeArticleLookupFunction;
            class TrGrammarSynthesizer_TrDisplayFunction;
        } // synthesis
    } // grammar
} // inflection
