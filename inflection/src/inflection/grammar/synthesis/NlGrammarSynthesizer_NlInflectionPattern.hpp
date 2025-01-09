/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>

class inflection::grammar::synthesis:: NlGrammarSynthesizer_NlInflectionPattern
    : public virtual NlGrammarSynthesizer_InflectorPattern
{
public:
    typedef ::inflection::grammar::synthesis::NlGrammarSynthesizer_InflectorPattern super;

private:
    const ::inflection::dialog::SemanticFeature* declensionFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const ::inflection::dialog::SemanticFeature* caseFeature {  };
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };

public: /* protected */
    virtual bool isAdjective(const ::std::u16string& word) const;
    virtual bool isNoun(const ::std::u16string& word) const;
    static void logUnsupportedInflection(const ::std::u16string& displayString, const ::std::u16string& reason);
    virtual bool isValidDeclensionConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    virtual bool isValidCaseConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    virtual bool isValidCountConstraint(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model);

private:
    friend class NlGrammarSynthesizer_NlNounInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveListInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern;
    friend class NlGrammarSynthesizer_PossessivePattern;
};
