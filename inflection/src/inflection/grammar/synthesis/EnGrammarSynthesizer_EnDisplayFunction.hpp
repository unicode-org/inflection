/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dialog/DefinitenessDisplayFunction.hpp>
#include <inflection/dialog/StaticArticleLookupFunction.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::EnGrammarSynthesizer_EnDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& speakFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    int64_t pluralProperty {  };
    int64_t singularProperty {  };
    int64_t prepositionProperty {  };
    int64_t abbreviationProperty {  };
    std::u16string_view APOSTROPHES {  };
    std::u16string_view S_LETTERS {  };
    const ::inflection::dictionary::Inflector &inflector;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    ::inflection::dialog::StaticArticleLookupFunction definiteArticleLookupFunction;
    ::inflection::grammar::synthesis::EnGrammarSynthesizer_ArticleLookupFunction indefiniteArticleLookupFunction;
    ::inflection::dialog::DefinitenessDisplayFunction definitenessDisplayFunction;
    const ::std::map<::std::u16string_view, ::std::u16string_view> possessiveDeterminers;

private:
    ::std::u16string guessPluralInflection(const ::std::u16string& displayString) const;
    ::std::u16string guessSingularInflection(const ::std::u16string& displayString) const;
    ::std::u16string inflectPossessive(const std::u16string &displayString, std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &valueConstraints, bool isRequestingPlural) const;
    ::std::optional<::std::u16string> inflectPhrase(const std::u16string &originalString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    explicit EnGrammarSynthesizer_EnDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~EnGrammarSynthesizer_EnDisplayFunction() override;
    EnGrammarSynthesizer_EnDisplayFunction(const EnGrammarSynthesizer_EnDisplayFunction&) = delete;
    EnGrammarSynthesizer_EnDisplayFunction& operator=(const EnGrammarSynthesizer_EnDisplayFunction&) = delete;
};
