/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/analysis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <string_view>

class inflection::analysis::RussianExposableMorphology final
    : public ::inflection::dialog::DictionaryLookupInflector
{
public:
    typedef ::inflection::dialog::DictionaryLookupInflector super;

public:
    ::icu4cxx::UnicodeSet inflectableChars { u"[\\u002d\\u0020]" };

public:
    bool isInflectable(::std::u16string_view word) const;

public:
    static RussianExposableMorphology* getInstance();

private:
    explicit RussianExposableMorphology();
    ~RussianExposableMorphology() override;
};
