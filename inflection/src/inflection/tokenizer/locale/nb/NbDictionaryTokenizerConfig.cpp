/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/nb/NbDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace nb {

NbDictionaryTokenizerConfig::NbDictionaryTokenizerConfig()
    : super({u"s", u"e"}, {}, {}, {u"en", u"n"})
{
    setMinCandidateLength(2);
    setMinSegmentLength(3);
    setMaxCompoundFreq(4000);
    setLowerMinScoreRatio(0.25f);
    setUpperMinScoreRatio(0.6f);
    setMinCompoundLengthForCredibility(20);
}

} // namespace tokenizer::locale::nb
} // namespace inflection
