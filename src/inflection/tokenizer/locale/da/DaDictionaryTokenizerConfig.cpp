/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/da/DaDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace da {

DaDictionaryTokenizerConfig::DaDictionaryTokenizerConfig()
    : super({u"s",  u"e", u"er"}, {}, {}, {u"en", u"n"})
{
    setMinCandidateLength(3);
    setMinSegmentLength(4);
    setMaxCompoundFreq(4000);
    setLowerMinScoreRatio(0.25f);
    setUpperMinScoreRatio(0.6f);
    setMinCompoundLengthForCredibility(20);
}

} // namespace tokenizer::locale::da
} // namespace inflection
