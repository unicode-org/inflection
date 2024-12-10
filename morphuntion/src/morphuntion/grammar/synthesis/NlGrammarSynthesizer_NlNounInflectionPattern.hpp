/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlInflectionPattern.hpp>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_NlNounInflectionPattern
    : public NlGrammarSynthesizer_NlInflectionPattern
{
public:
    typedef NlGrammarSynthesizer_NlInflectionPattern super;

private:
    const ::morphuntion::dictionary::Inflector &inflector;
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };

public:
    bool isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    ::std::u16string inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    explicit NlGrammarSynthesizer_NlNounInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model);
};
