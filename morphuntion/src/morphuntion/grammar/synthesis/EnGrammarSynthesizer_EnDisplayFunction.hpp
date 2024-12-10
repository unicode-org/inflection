/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/dialog/DefinitenessDisplayFunction.hpp>
#include <morphuntion/dialog/StaticArticleLookupFunction.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::EnGrammarSynthesizer_EnDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* speakFeature {  };
    const ::morphuntion::dialog::SemanticFeature* partOfSpeechFeature {  };
    const ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    int64_t pluralProperty {  };
    int64_t singularProperty {  };
    int64_t prepositionProperty {  };
    int64_t abbreviationProperty {  };
    std::u16string_view APOSTROPHES {  };
    std::u16string_view S_LETTERS {  };
    const ::morphuntion::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    ::morphuntion::dialog::StaticArticleLookupFunction definiteArticleLookupFunction;
    ::morphuntion::grammar::synthesis::EnGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::morphuntion::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;

private:
    ::std::u16string guessPluralInflection(const ::std::u16string& displayString) const;
    ::std::u16string guessSingularInflection(const ::std::u16string& displayString) const;
    ::std::u16string inflectPossessive(const std::u16string &displayString, std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &valueConstraints) const;
    ::std::optional<::std::u16string> inflectPhrase(const std::u16string &originalString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;


public: /* package */
    explicit EnGrammarSynthesizer_EnDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~EnGrammarSynthesizer_EnDisplayFunction() override;


private:
    static const ::std::map<::std::u16string_view, ::std::u16string_view>& POSSESSIVE_DETERMINERS();

private:
    EnGrammarSynthesizer_EnDisplayFunction(EnGrammarSynthesizer_EnDisplayFunction&) = delete;
    EnGrammarSynthesizer_EnDisplayFunction& operator=(const EnGrammarSynthesizer_EnDisplayFunction &) = delete;

};
