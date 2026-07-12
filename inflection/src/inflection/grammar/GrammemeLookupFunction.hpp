/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/grammar/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <functional>
#include <string>
#include <vector>

class inflection::grammar::GrammemeLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;
    using SuffixFunction = ::std::function<::std::u16string(const ::std::u16string&)>;

protected:
    const int64_t relevantGrammemes {  };

private:
    ::std::u16string defaultValue {  };
    bool firstWordDeterminesValue {  };
    SuffixFunction suffixFunction {  };

public:
    ::std::u16string determine(const ::std::u16string& word) const override;

    GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech);
    GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech, bool firstWordDeterminesValue);
    GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::u16string& defaultValue, bool firstWordDeterminesValue);
    GrammemeLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags, const ::std::vector<::std::u16string>& disambiguationPartsOfSpeech, const ::std::u16string& defaultValue, bool firstWordDeterminesValue, SuffixFunction suffixFunction);
    ~GrammemeLookupFunction() override;
    GrammemeLookupFunction(const GrammemeLookupFunction&) = delete;
    GrammemeLookupFunction& operator=(const GrammemeLookupFunction&) = delete;
};
