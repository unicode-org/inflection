/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>

class inflection::grammar::synthesis::UrGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);
private:
    UrGrammarSynthesizer() = delete;
};
