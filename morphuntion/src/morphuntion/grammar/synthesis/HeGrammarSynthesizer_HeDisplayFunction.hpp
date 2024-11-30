/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupInflector.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <string>
#include <set>
#include <vector>

class morphuntion::grammar::synthesis::HeGrammarSynthesizer_HeDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::morphuntion::dictionary::Inflector &inflector;
    ::morphuntion::dialog::DictionaryLookupInflector dictionaryInflector;
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* definitenessFeature {  };
    int64_t genderMask {  };
    int64_t nounProperty {  };
    int64_t adjectiveProperty {  };
    int64_t dictionaryPreposition {  };

private:
    std::optional<::std::u16string> pluralizeFirstWordOfCompoundWithInflector(const ::std::u16string& firstWord) const;
    ::std::u16string pluralizeFirstWordOfCompoundWithHeuristics(const ::std::u16string& firstWord) const;
    ::std::u16string inflectCompoundToPlural(const ::std::u16string& stemmedSingular) const;

public:
    virtual ::std::u16string singleWordStemToPlural(::std::u16string_view stemmedSingular) const;

private:
    ::std::u16string inflectDisplayString(const ::std::u16string& displayString, const std::u16string &count, const std::u16string &gender) const;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    ::std::u16string applyDefiniteness(const ::std::u16string& input, ::std::u16string_view definiteness) const;

public: /* package */
    explicit HeGrammarSynthesizer_HeDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
};
