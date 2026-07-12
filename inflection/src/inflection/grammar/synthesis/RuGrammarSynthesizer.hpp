/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>
#include <string_view>

class inflection::grammar::synthesis::RuGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

public:
    static bool startsWith2Consonant(const ::std::u16string& word, std::u16string_view firstLowercaseLetters);

private:
    RuGrammarSynthesizer() = delete;

public:
    static constexpr auto PREPOSITION_ABOUT = u"aboutPrep";
    static constexpr auto PREPOSITION_IN = u"inPrep";
    static constexpr auto PREPOSITION_WITH = u"withPrep";
    static constexpr auto PREPOSITION_TO = u"toPrep";
    static constexpr auto WITH_PREPOSITION_ABOUT = u"withAboutPrep";
    static constexpr auto WITH_PREPOSITION_IN = u"withInPrep";
    static constexpr auto WITH_PREPOSITION_WITH = u"withWithPrep";
    static constexpr auto WITH_PREPOSITION_TO = u"withToPrep";
};
