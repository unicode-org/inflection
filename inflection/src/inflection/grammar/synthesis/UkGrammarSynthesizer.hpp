/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <stdint.h>
#include <string_view>
#include <tuple>
#include <vector>

class inflection::grammar::synthesis::UkGrammarSynthesizer final
{
public:
    static std::vector<std::tuple<std::u16string_view, int64_t, int64_t>> getPrefixesWithPos(const ::inflection::dictionary::DictionaryMetaData& dictionary);
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);
private:
    UkGrammarSynthesizer() = delete;
};
