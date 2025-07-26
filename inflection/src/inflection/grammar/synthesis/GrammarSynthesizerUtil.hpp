/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/fwd.hpp>
#include <string>
#include <map>
#include <vector>

class inflection::grammar::synthesis::GrammarSynthesizerUtil final
{
public:
    static bool checkSignificantTokenForInflection(const ::inflection::tokenizer::Token &token);
    static ::std::vector<::std::u16string> getSignificantWords(const ::inflection::tokenizer::TokenChain& tokenChain);
    static std::u16string getStringFromInflectedSignificantWords(const inflection::tokenizer::TokenChain& tokenChain, ::std::vector<::std::u16string> inflectedSignificantWords);
    static ::std::u16string inflectSignificantWords(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain, const ::inflection::grammar::synthesis::SignificantTokenInflector& inflector);
    static ::std::u16string getFeatureValue(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature& semanticFeature);
    static bool hasFeature(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::dialog::SemanticFeature* semanticFeature);
    static bool hasAnyFeatures(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::vector<const ::inflection::dialog::SemanticFeature*> &semanticFeatures);
    static ::std::vector<::std::u16string> convertToStringConstraints(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::vector<const ::inflection::dialog::SemanticFeature*> &semanticFeatures);
    // Searches in the displayData and returns the index of the displayValue with maximum matching constraints in the array of displayValues
    static const dialog::DisplayValue* getTheBestDisplayValue(const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints);
    static ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> mergeConstraintsWithDisplayValue (const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints);
    static void mergeConstraintsWithExisting(::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &existingConstraints, const ::std::vector<::std::pair<const ::inflection::dialog::SemanticFeature* const, ::std::u16string>> &newConstraints);
    static void inflectAndAppendArticlePrefix(::std::u16string &displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &displayValueConstraints, const ::inflection::dialog::DefaultArticleLookupFunction *articleLookupFunction, const ::inflection::dialog::DefaultArticleLookupFunction::ArticleDisplayValue* articleDisplayValue);
    static const inflection::tokenizer::Token* getFirstSignificantToken(const inflection::tokenizer::Token* tok);
    static const inflection::tokenizer::Token* getLastSignificantToken(const inflection::tokenizer::Token* tok);

private:
    GrammarSynthesizerUtil() = delete;
};
