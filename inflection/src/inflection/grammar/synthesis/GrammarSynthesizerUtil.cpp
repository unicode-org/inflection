/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>

#include <inflection/grammar/synthesis/SignificantTokenInflector.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/npc.hpp>
#include <algorithm>
#include <map>
#include <memory>

namespace inflection::grammar::synthesis {

using ::inflection::tokenizer::TokenChain;
using ::inflection::dialog::SemanticFeature;
using ::inflection::dialog::DefaultFeatureFunction;
using ::inflection::dialog::DefaultArticleLookupFunction;
using ::inflection::dialog::DisplayValue;
using ::inflection::dialog::SpeakableString;

::std::vector<::std::u16string> GrammarSynthesizerUtil::getSignificantWords(const TokenChain& tokenChain)
{
    ::std::vector<::std::u16string> words;
    words.reserve(tokenChain.getWordCount());
    for (const auto& tNext : tokenChain) {
        if (checkSignificantTokenForInflection(tNext)) {
            words.emplace_back(tNext.getValue());
        }
    }
    return words;
}

std::u16string GrammarSynthesizerUtil::getStringFromInflectedSignificantWords(const TokenChain& tokenChain, ::std::vector<::std::u16string> inflectedSignificantWords) {
    ::std::u16string inflectionResult;
    int idx = 0;
    for (const auto& token : tokenChain) {
        if (checkSignificantTokenForInflection(token)) {
            inflectionResult += inflectedSignificantWords.at(idx++);
        } else {
            inflectionResult += token.getValue();
        }
    }
    return inflectionResult;
}

::std::u16string GrammarSynthesizerUtil::inflectSignificantWords(const ::std::map<SemanticFeature, ::std::u16string>& constraints, const TokenChain& tokenChain, const SignificantTokenInflector& inflector)
{
    int32_t countI = 0;
    ::std::vector<::std::u16string> tokens(tokenChain.getSize());
    ::std::vector<int32_t> indexesOfSignificantWords;
    indexesOfSignificantWords.reserve(tokenChain.getSize());
    for (const auto& tNext : tokenChain) {
        const auto& nextValue = tNext.getValue();
        tokens[countI] = nextValue;
        if (checkSignificantTokenForInflection(tNext)) {
            // the token is significant
            indexesOfSignificantWords.emplace_back(countI);
        }
        countI++;
    }

    if (indexesOfSignificantWords.empty()) {
        return tokenChain.toOriginatingString();
    }

    auto inflectedTokens = inflector.inflectSignificantTokens(constraints, &tokens, indexesOfSignificantWords);
    if (inflectedTokens.empty()) {
        return tokenChain.toOriginatingString();
    }
    ::std::u16string inflectedResult;
    for (const auto& token : tokens) {
        inflectedResult.append(token);
    }
    return inflectedResult;
}

bool GrammarSynthesizerUtil::hasFeature(const ::std::map<SemanticFeature, ::std::u16string>& constraints, const SemanticFeature* semanticFeature) {
    return constraints.find(*npc(semanticFeature)) != constraints.end();
}

bool GrammarSynthesizerUtil::hasAnyFeatures(const std::map<SemanticFeature, ::std::u16string> &constraints, const std::vector<const SemanticFeature *> &semanticFeatures) {
    return ::std::any_of(semanticFeatures.begin(), semanticFeatures.end(), [&constraints](const auto semanticFeature) {
        return GrammarSynthesizerUtil::hasFeature(constraints, semanticFeature);
    });
}

::std::u16string GrammarSynthesizerUtil::getFeatureValue(const ::std::map<SemanticFeature, ::std::u16string>& constraints, const SemanticFeature& semanticFeature)
{
    auto result = constraints.find(semanticFeature);
    if (result != constraints.end()) {
        return result->second;
    }
    return {};
}

::std::vector<::std::u16string> GrammarSynthesizerUtil::convertToStringConstraints(const ::std::map<SemanticFeature, ::std::u16string>& constraints, const ::std::vector<const SemanticFeature*>& semanticFeatures) {
    ::std::vector<::std::u16string> result;
    for (const auto semanticFeature : semanticFeatures) {
        const auto &constraintString = GrammarSynthesizerUtil::getFeatureValue(constraints, *npc(semanticFeature));
        if (constraintString.empty()) {
            continue;
        }
        result.emplace_back(constraintString);
    }
    return result;
}

bool GrammarSynthesizerUtil::checkSignificantTokenForInflection(const ::inflection::tokenizer::Token &token) {
    const auto &word = token.getValue();
    return !(!token.isSignificant() || token.isWhitespace() || word == u"-" || word.empty());
}

const DisplayValue* GrammarSynthesizerUtil::getTheBestDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const std::map<SemanticFeature, ::std::u16string> &constraints) {
    const DisplayValue* choosenDisplayValue = nullptr;
    int32_t maxConstraintsMatch = -1;
    for (const auto& displayValue : displayData.getValues()) {
        int32_t numConstraintsMatch = 0;
        for (const auto& displayDataConstraint : displayValue.getConstraintMap()) {
            auto constraintValue = constraints.find(displayDataConstraint.first);
            if (constraintValue != constraints.end() && constraintValue->second == displayDataConstraint.second) {
                numConstraintsMatch++;
            }
        }
        if (numConstraintsMatch > maxConstraintsMatch) {
            choosenDisplayValue = &displayValue;
            maxConstraintsMatch = numConstraintsMatch;
        }
    }
    return choosenDisplayValue;
}

::std::map<SemanticFeature, ::std::u16string> GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(const dialog::DisplayValue &displayValue, const std::map<SemanticFeature, ::std::u16string> &constraints) {
    auto mergedConstraints(constraints);
    const auto& displayDataConstraints(displayValue.getConstraintMap());
    mergedConstraints.insert(displayDataConstraints.begin(), displayDataConstraints.end());
    return mergedConstraints;
}

void GrammarSynthesizerUtil::mergeConstraintsWithExisting(::std::map<SemanticFeature, ::std::u16string> &existingConstraints, const ::std::vector<::std::pair<const SemanticFeature* const, ::std::u16string>> &newConstraints) {
    for (const auto &[constraintFeature, constraintValue]: newConstraints) {
        if (constraintFeature != nullptr) {
            existingConstraints.insert({*constraintFeature, constraintValue});
        }
    }
}

void GrammarSynthesizerUtil::inflectAndAppendArticlePrefix(::std::u16string &displayString, const ::std::map<SemanticFeature, ::std::u16string> &displayValueConstraints, const DefaultArticleLookupFunction *articleLookupFunction, const DefaultArticleLookupFunction::ArticleDisplayValue* articleDisplayValue) {
    if (articleLookupFunction != nullptr) {
        auto articleConstraints(displayValueConstraints);
        GrammarSynthesizerUtil::mergeConstraintsWithExisting(articleConstraints, articleDisplayValue->second);
        const DefaultFeatureFunction *featureFunction = articleLookupFunction;
        ::std::unique_ptr<DisplayValue> inflectedDisplayValue(new DisplayValue(displayString, articleConstraints));
        ::std::unique_ptr<SpeakableString> speakableDisplayString(featureFunction->getFeatureValue(*inflectedDisplayValue, {}));
        if (speakableDisplayString != nullptr) {
            displayString = speakableDisplayString->getPrint();
        }
    }
}

const inflection::tokenizer::Token* GrammarSynthesizerUtil::getFirstSignificantToken(const inflection::tokenizer::Token* tok)
{
    while (tok != nullptr) {
        if (tok->isSignificant()) {
            return tok;
        }
        tok = tok->getNext();
    }
    return nullptr;
}

const inflection::tokenizer::Token* GrammarSynthesizerUtil::getLastSignificantToken(const inflection::tokenizer::Token* tok)
{
    while (tok != nullptr) {
        if (tok->isSignificant()) {
            return tok;
        }
        tok = tok->getPrevious();
    }
    return nullptr;
}

} // namespace inflection::grammar::synthesis
