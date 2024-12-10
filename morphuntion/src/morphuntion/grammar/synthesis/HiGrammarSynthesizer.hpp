/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::HiGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);
private:
    HiGrammarSynthesizer() = delete;
};
