/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <morphuntion/tokenizer/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::ArGrammarSynthesizer_ArDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    struct InflectionConstraints {
        ::std::u16string caseString;
        ::std::u16string countString;
        ::std::u16string genderString;
        ::std::u16string definitenessString;
        ::std::u16string pronounCountString;
        ::std::u16string pronounGenderString;
        ::std::u16string partOfSpeechString;
        ::std::vector<::std::u16string> disambiguationGrammemeValues;
    };

    ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::morphuntion::dialog::SemanticFeature& caseFeature;
    const ::morphuntion::dialog::SemanticFeature& countFeature;
    const ::morphuntion::dialog::SemanticFeature& genderFeature;
    const ::morphuntion::dialog::SemanticFeature& partOfSpeechFeature;
    const ::morphuntion::dialog::SemanticFeature& definitenessFeature;
    const ::morphuntion::dialog::SemanticFeature& pronounCountFeature;
    const ::morphuntion::dialog::SemanticFeature& pronounGenderFeature;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    ::std::map<int32_t, ::std::u16string_view> personalPronounsMap {  };
    ::std::map<::std::u16string_view, int32_t> personalPronounsReverseMap;
    int64_t posMask;
    int64_t verbMask;
    int64_t determinerMask;

private:
    ::std::u16string inflectString(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string lookupDefiniteArticle(const ::std::u16string& displayString);
    ::std::u16string performInflection(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const;
    ::std::u16string performPronounInflection(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const;

public: /* package */
    explicit ArGrammarSynthesizer_ArDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& personalPronounsMap);
};
