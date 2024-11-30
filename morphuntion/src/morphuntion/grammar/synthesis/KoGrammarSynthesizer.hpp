/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <icu4cxx/UnicodeSet.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::KoGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);

private:
    KoGrammarSynthesizer() = delete;

public: /* package */
    static const icu4cxx::UnicodeSet& EMPTY_SET();
    static const icu4cxx::UnicodeSet& NONLATIN_VOWELS();
    static const icu4cxx::UnicodeSet& VOWELS_WITH_RIEUL();
    static const icu4cxx::UnicodeSet& DEFAULT_RIEUL_END();
};
