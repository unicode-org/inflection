/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::ArGrammarSynthesizer_ArDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    struct InflectionConstraints {
        ::std::u16string caseString {  };
        ::std::u16string moodString {  };
        ::std::u16string countString {  };
        ::std::u16string tenseString {  };
        ::std::u16string personString {  };
        ::std::u16string genderString {  };
        ::std::u16string animacyString {  };
        ::std::u16string definitenessString {  };
        ::std::u16string pronounCountString {  };
        ::std::u16string pronounGenderString {  };
        ::std::u16string partOfSpeechString {  };
        ::std::vector<::std::u16string> disambiguationGrammemeValues {  };
    };

    ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& moodFeature;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& tenseFeature;
    const ::inflection::dialog::SemanticFeature& personFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& animacyFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dialog::SemanticFeature& definitenessFeature;
    const ::inflection::dialog::SemanticFeature& pronounCountFeature;
    const ::inflection::dialog::SemanticFeature& pronounGenderFeature;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    ::std::map<int32_t, ::std::u16string_view> personalPronounsMap {  };
    ::std::map<::std::u16string_view, int32_t> personalPronounsReverseMap {  };
    int64_t posMask {  };
    int64_t verbMask {  };
    int64_t determinerMask {  };

private:
    ::std::u16string inflectString(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const;

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string lookupDefiniteArticle(const ::std::u16string& displayString);
    ::std::u16string performInflection(const ::std::u16string &word, int64_t wordGrammemes, const InflectionConstraints &inflectionContraints) const;
    ::std::u16string performPronounInflection(const ::std::u16string &word, const InflectionConstraints &inflectionContraints) const;

public: /* package */
    explicit ArGrammarSynthesizer_ArDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& personalPronounsMap);
    ArGrammarSynthesizer_ArDisplayFunction(const ArGrammarSynthesizer_ArDisplayFunction&) = delete;
    ArGrammarSynthesizer_ArDisplayFunction& operator=(const ArGrammarSynthesizer_ArDisplayFunction&) = delete;
};
