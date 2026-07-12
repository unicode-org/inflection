/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/fwd.hpp>
#include <string>

class inflection::grammar::PrefixedDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& extraFeature;
    const ::inflection::dialog::SemanticFeature& partOfSpeechFeature;
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;
    std::vector<std::tuple<std::u16string_view, int64_t, int64_t>> prefixesWithPOS {  };

private:
    ::std::optional<::std::u16string> inflectWord(::std::u16string_view word, int64_t& wordType, const ::std::vector<::std::u16string>& constraints, const ::std::vector<::std::u16string>& disambiguationGrammemeValues, bool enableInflectionGuess) const;
public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public: /* package */
    using PrefixGenerator = std::vector<std::tuple<std::u16string_view, int64_t, int64_t>>(*)(const ::inflection::dictionary::DictionaryMetaData&);

    PrefixedDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::inflection::util::ULocale& locale, const ::std::vector<::std::vector<::std::u16string_view>>& grammemeData, const ::std::u16string& extraFeatureName, PrefixGenerator prefixGenerator = nullptr);
    ~PrefixedDisplayFunction() override;
    PrefixedDisplayFunction(const PrefixedDisplayFunction&) = delete;
    PrefixedDisplayFunction& operator=(const PrefixedDisplayFunction&) = delete;
};
