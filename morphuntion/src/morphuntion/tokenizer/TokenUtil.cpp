/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/TokenUtil.hpp>

#include <morphuntion/tokenizer/Token.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>
#include <unicode/uchar.h>

namespace morphuntion::tokenizer {

const ::icu4cxx::UnicodeSet &TokenUtil::DELIMITER_CHARACTERS()
{
    static auto DELIMITER_CHARACTERS_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[\\p{Separator}\\p{Control}\\p{Word_Break=format}'’·・]"));
    return *npc(DELIMITER_CHARACTERS_);
}

const ::icu4cxx::UnicodeSet &TokenUtil::SIGNIFICANT_CHARACTERS()
{
    static auto SIGNIFICANT_CHARACTERS_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[\\&\\p{Letter}\\p{Number}\\p{Symbol}]"));
    return *npc(SIGNIFICANT_CHARACTERS_);
}

const ::icu4cxx::UnicodeSet &TokenUtil::PRIOR_TO_DASH_DELIMITER()
{
    static auto PRIOR_TO_DASH_DELIMITER_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:Letter:]-[:Common:]-[:Thai:]-[:Hiragana:]-[:Katakana:]-[:Khmer:]][[:Nonspacing_Mark:]&[:Zinh:]]]"));
    return *npc(PRIOR_TO_DASH_DELIMITER_);
}

bool TokenUtil::containsAllDelimitors(std::u16string_view s)
{
    return DELIMITER_CHARACTERS().containsAllCodePoints(s);
}

bool TokenUtil::isSignificant(std::u16string_view value)
{
    auto length = value.length();
    if (length > 1) {
        return true;
    }
    if (length == 0) {
        return false;
    }
    UChar32 cp;
    U16_GET(value, 0, 0, length, cp);
    return SIGNIFICANT_CHARACTERS().contains(cp);
}

bool TokenUtil::isDelimiterPriorToDash(Token* token)
{
    return token != nullptr && npc(token)->getLength() >= 1 && PRIOR_TO_DASH_DELIMITER().contains(npc(token)->getValue()[0]);
}

} // namespace morphuntion::tokenizer
