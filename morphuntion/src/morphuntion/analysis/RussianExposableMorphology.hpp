/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/fwd.hpp>
#include <morphuntion/analysis/fwd.hpp>
#include <morphuntion/analysis/MorphologicalAnalyzer.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <optional>
#include <set>
#include <string>
#include <vector>

class morphuntion::analysis::RussianExposableMorphology final
    : public ::morphuntion::dialog::DictionaryLookupInflector
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupInflector super;

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
    ::icu4cxx::UnicodeSet inflectableChars { u"[\u002d]" }; //Represents the character "-"

private:
    bool isInflectable(::std::u16string_view word) const;

    bool isNoun(int64_t phraseType) const;
    bool isAdjective(int64_t phraseType) const;
    bool isNumber(int64_t phraseType) const;
    bool isProperNoun(int64_t phraseType) const;

public:
    ::std::optional<::std::u16string> inflectUsingDictionary(const ::std::u16string &word, const ::std::u16string &caseString, const ::std::u16string &countString, const ::std::u16string &genderString, const ::std::u16string &animacyString, const ::std::u16string &posString) const;

public:
    explicit RussianExposableMorphology();
    ~RussianExposableMorphology() override;
};
