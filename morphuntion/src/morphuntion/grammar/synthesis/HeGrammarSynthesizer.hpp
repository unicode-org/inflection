/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::HeGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);

private:
    HeGrammarSynthesizer() = delete;

public:
    static constexpr auto WITH_CONDITIONAL_HYPHEN = u"withConditionalHyphen";
};
