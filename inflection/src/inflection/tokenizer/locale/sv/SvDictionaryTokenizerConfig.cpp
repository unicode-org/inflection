/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/sv/SvDictionaryTokenizerConfig.hpp>

namespace inflection::tokenizer::locale {
namespace sv {


SvDictionaryTokenizerConfig::SvDictionaryTokenizerConfig()
    : super({u"s", u"e"}, {u"e", u"o", u"u", u"a"}, {u"a", u"e", u"i", u"o", u"u"}, {u"e"})
{
    setMinCandidateLength(2);
    setMinSegmentLength(3);
    setMaxCompoundFreq(2000);
    setLowerMinScoreRatio(0.4f);
    setUpperMinScoreRatio(0.75f);
    setMinCompoundLengthForCredibility(8);
}

} // namespace tokenizer::locale::sv
} // namespace inflection
