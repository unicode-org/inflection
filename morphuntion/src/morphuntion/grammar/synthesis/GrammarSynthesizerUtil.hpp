/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <string>
#include <map>
#include <vector>

class morphuntion::grammar::synthesis::GrammarSynthesizerUtil final
{
public:
    static bool checkSignificantTokenForInflection(const ::morphuntion::tokenizer::Token &token);
    static ::std::vector<::std::u16string> getSignificantWords(const ::morphuntion::tokenizer::TokenChain& tokenChain);
    static std::u16string getStringFromInflectedSignificantWords(const morphuntion::tokenizer::TokenChain& tokenChain, ::std::vector<::std::u16string> inflectedSignificantWords);
    static ::std::u16string inflectSignificantWords(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::tokenizer::TokenChain& tokenChain, const ::morphuntion::grammar::synthesis::GrammarSynthesizerUtil_SignificantTokenInflector& inflector);
    static ::std::u16string getFeatureValue(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::dialog::SemanticFeature* semanticFeature);
    static bool hasFeature(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::morphuntion::dialog::SemanticFeature* semanticFeature);
    static bool hasAnyFeatures(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::vector<const ::morphuntion::dialog::SemanticFeature*> &semanticFeatures);
    static ::std::vector<::std::u16string> convertToStringConstraints(const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::vector<const ::morphuntion::dialog::SemanticFeature*> &semanticFeatures);
    // Searches in the displayData and returns the index of the displayValue with maximum matching constraints in the array of displayValues
    static const dialog::SemanticFeatureModel_DisplayValue* getTheBestDisplayValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints);
    static ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> mergeConstraintsWithDisplayValue (const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints);
    static void mergeConstraintsWithExisting(::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &existingConstraints, const ::std::vector<::std::pair<const ::morphuntion::dialog::SemanticFeature* const, ::std::u16string>> &newConstraints);
    static void inflectAndAppendArticlePrefix(::std::u16string &displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &displayValueConstraints, const ::morphuntion::dialog::DefaultArticleLookupFunction *articleLookupFunction, const ::morphuntion::dialog::DefaultArticleLookupFunction::ArticleDisplayValue* articleDisplayValue);

private:
    GrammarSynthesizerUtil() = delete;
};
