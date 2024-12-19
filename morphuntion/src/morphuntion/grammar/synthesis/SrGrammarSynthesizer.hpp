/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>
#include <set>

class morphuntion::grammar::synthesis::SrGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);

private:
    SrGrammarSynthesizer() = delete;
};
