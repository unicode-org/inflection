/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/analysis/fwd.hpp>
#include <inflection/analysis/MorphologicalAnalyzer.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <optional>
#include <set>
#include <string>
#include <vector>

class inflection::analysis::RussianExposableMorphology final
    : public ::inflection::dialog::DictionaryLookupInflector
{
public:
    typedef ::inflection::dialog::DictionaryLookupInflector super;

private:
    //POS
    int64_t dictionaryNoun {  };
    int64_t dictionaryProperNoun {  };
    int64_t dictionaryAdjective {  };
    int64_t dictionaryAdverb {  };
    int64_t dictionaryVerb {  };
    int64_t dictionaryNumber {  };
    int64_t dictionaryPOSMask {  };

    ::std::set<::std::u16string_view> doNotInflect { u"января", u"февраля", u"марта", u"апреля", u"мая", u"июня", u"июля", u"августа", u"сентября", u"октября", u"ноября", u"декабря", u"мин" };

public:
    ::icu4cxx::UnicodeSet inflectableChars { u"[\u002d]" }; // Represents the character "-"

private:
    bool isUnlemmatizable(int64_t phraseType) const;
    bool isLemmatizableDict(int64_t phraseType) const;
    bool isNoun(int64_t phraseType) const;
    bool isProperNoun(int64_t phraseType) const;
    bool isInflectableByMachineLearning(int64_t phraseType) const;
    bool isInflectable(::std::u16string_view word) const;

public:
    ::std::optional<::std::u16string> inflectUsingDictionary(const ::std::u16string &word, const ::std::u16string &caseString, const ::std::u16string &countString, const ::std::u16string &genderString, const ::std::u16string &animacyString, const ::std::u16string &posString) const;

public:
    explicit RussianExposableMorphology();
    ~RussianExposableMorphology() override;
};
