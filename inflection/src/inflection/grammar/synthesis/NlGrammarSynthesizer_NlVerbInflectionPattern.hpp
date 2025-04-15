/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlVerbInflectionPattern
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private:
    const ::inflection::dictionary::Inflector &inflector;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryVerb {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };

public:
    ::std::u16string inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

private:
    NlGrammarSynthesizer::Number getGrammaticalNumber(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlVerbInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlVerbInflectionPattern() override;

private:
    friend class NlGrammarSynthesizer_NlDisplayFunction;
};
