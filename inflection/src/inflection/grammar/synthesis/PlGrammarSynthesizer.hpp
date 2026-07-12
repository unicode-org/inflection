/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::PlGrammarSynthesizer final
{
public:
    static std::u16string getPreposition(const std::u16string& preposition, std::u16string_view word);
    static std::u16string getPreposition(const inflection::tokenizer::Token* tok);
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);
private:
    PlGrammarSynthesizer() = delete;
};
