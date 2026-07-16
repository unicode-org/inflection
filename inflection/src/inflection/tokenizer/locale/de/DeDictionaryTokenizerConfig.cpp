/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/de/DeDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace de {

DeDictionaryTokenizerConfig::DeDictionaryTokenizerConfig()
    : super({u"s", u"se", u"e", u"n", u"ne", u"re", u"sne"}, {}, {}, {})
{
    setMinCandidateLength(3);
    setMinSegmentLength(3);
    setMaxCompoundFreq(4000);
    setLowerMinScoreRatio(0.25f);
    setUpperMinScoreRatio(0.6f);
    setMinCompoundLengthForCredibility(20);
}

} // namespace tokenizer::locale::de
} // namespace inflection
