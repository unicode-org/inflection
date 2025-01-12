/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlNounInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    const ::inflection::dictionary::Inflector &inflector;
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    NlGrammarSynthesizer_NlNounInflectionPattern(const NlGrammarSynthesizer_NlNounInflectionPattern&) = delete;
    NlGrammarSynthesizer_NlNounInflectionPattern& operator=(const NlGrammarSynthesizer_NlNounInflectionPattern&) = delete;
};
