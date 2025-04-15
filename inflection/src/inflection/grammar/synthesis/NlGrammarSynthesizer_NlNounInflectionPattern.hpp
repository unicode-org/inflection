/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>

class inflection::grammar::synthesis::NlGrammarSynthesizer_NlNounInflectionPattern
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private:
    const ::inflection::dictionary::Inflector &inflector;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& declensionFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryNoun {  };
    int64_t dictionarySingular {  };
    int64_t dictionaryPlural {  };
    ::std::u16string_view apostrophes { u"‘’‛'′´❛❜" };
    const ::std::map<::std::u16string_view, ::std::u16string_view> possessivePronouns {  };
    const ::std::set<::std::u16string_view> countConditionalPronouns {  };
    const ::std::set<::std::u16string_view> declensionConditional {  };

public:
    bool isGenitiveRequested(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    ::std::u16string inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

private:
    ::std::u16string inflectGenitive(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    bool isApostrophe(char16_t c) const;
    bool isPossessive(const ::std::u16string& text) const;
    static bool endsWithDutchSSound(const ::std::u16string& text);
    bool isValidCaseConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    NlGrammarSynthesizer::Number getGrammaticalNumber(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
public:
    NlGrammarSynthesizer::Declension getDeclension(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlNounInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);
    ~NlGrammarSynthesizer_NlNounInflectionPattern() override;

private:
    friend class NlGrammarSynthesizer_NlDisplayFunction;
};
