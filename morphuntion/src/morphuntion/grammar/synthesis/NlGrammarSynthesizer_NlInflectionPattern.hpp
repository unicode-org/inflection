/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_InflectorPattern.hpp>

class morphuntion::grammar::synthesis:: NlGrammarSynthesizer_NlInflectionPattern
    : public virtual NlGrammarSynthesizer_InflectorPattern
{
public:
    typedef ::morphuntion::grammar::synthesis::NlGrammarSynthesizer_InflectorPattern super;

private:
    const ::morphuntion::dialog::SemanticFeature* declensionFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t dictionaryAdjective {  };
    int64_t dictionaryNoun {  };

public: /* protected */
    virtual bool isAdjective(const ::std::u16string& word) const;
    virtual bool isNoun(const ::std::u16string& word) const;
    static void logUnsupportedInflection(const ::std::u16string& displayString, const ::std::u16string& reason);
    virtual bool isValidDeclensionConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    virtual bool isValidCaseConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    virtual bool isValidCountConstraint(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public: /* package */
    explicit NlGrammarSynthesizer_NlInflectionPattern(const ::morphuntion::dialog::SemanticFeatureModel& model);

private:
    friend class NlGrammarSynthesizer_NlNounInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveListInflectionPattern;
    friend class NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern;
    friend class NlGrammarSynthesizer_PossessivePattern;
};
