/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::KoGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

private:
    KoGrammarSynthesizer() = delete;

public: /* package */
    static const icu4cxx::UnicodeSet& EMPTY_SET();
    static const icu4cxx::UnicodeSet& NONLATIN_VOWELS();
    static const icu4cxx::UnicodeSet& VOWELS_WITH_RIEUL();
    static const icu4cxx::UnicodeSet& DEFAULT_RIEUL_END();
};
