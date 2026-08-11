/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::HeGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

private:
    HeGrammarSynthesizer() = delete;

public:
    static constexpr auto WITH_CONDITIONAL_HYPHEN = u"withConditionalHyphen";
};
