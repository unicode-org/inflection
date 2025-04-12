/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/nl/NlDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace nl {


NlDictionaryTokenizerConfig::NlDictionaryTokenizerConfig()
    : super({u"s", u"e", u"ne"}, {}, {}, {})
{
    setMinCandidateLength(3);
    setMinSegmentLength(3);
    setMaxCompoundFreq(4000);
    setLowerMinScoreRatio(0.25f);
    setUpperMinScoreRatio(0.6f);
    setMinCompoundLengthForCredibility(20);
}

} // namespace tokenizer::locale::nl
} // namespace inflection
