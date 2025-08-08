/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#pragma once

#include <inflection/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/tokenizer/TokenExtractor.hpp>
#include <inflection/tokenizer/locale/DravidianWordAndDelimiterTokenExtractor_AffixRecord.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <vector>
#include <string_view>

class DravidianWordAndDelimiterTokenExtractor : public TokenExtractor {
public:
    DravidianWordAndDelimiterTokenExtractor();
    virtual ~DravidianWordAndDelimiterTokenExtractor();

    std::vector<Token> extract(const std::u16string& word) override;

    void setSuffixes(const std::vector<std::u16string>& suffixes);

private:
    std::vector<std::u16string> knownSuffixes;

    size_t findSuffixStart(const std::u16string& word) const;
};
