/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <string>
#include <set>

namespace inflection::grammar::synthesis {
    class MlGrammarSynthesizer_MlDisplayFunction : public virtual ::inflection::dialog::DefaultDisplayFunction {
    public:
        using super = ::inflection::dialog::DefaultDisplayFunction;

    private:
        const ::inflection::dialog::SemanticFeature& caseFeature;
        const ::inflection::dialog::SemanticFeature& numberFeature;
        const ::inflection::dialog::SemanticFeature& genderFeature;
        const ::inflection::dialog::SemanticFeature& posFeature;
        const ::inflection::dialog::SemanticFeature& formalityFeature;
        const ::inflection::dialog::SemanticFeature& clusivityFeature;
        const ::inflection::dialog::SemanticFeature& personFeature;
        const ::inflection::dialog::SemanticFeature& tenseFeature;
        const ::inflection::dialog::SemanticFeature& moodFeature;
        const ::inflection::dialog::SemanticFeature& pronounTypeFeature;
        const ::inflection::dialog::SemanticFeature& determinationFeature;
        ::inflection::dialog::DictionaryLookupInflector dictionaryInflector;

    public:
        ::inflection::dialog::DisplayValue* getDisplayValue(
            const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData,
            const std::map<::inflection::dialog::SemanticFeature, std::u16string>& constraints,
            bool enableInflectionGuess) const override;

        ::std::u16string inflectPhrase(
            const ::std::u16string& phrase,
            const ::std::vector<::std::u16string>& constraintValues,
            bool enableInflectionGuess) const;

    public:
        explicit MlGrammarSynthesizer_MlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
        ~MlGrammarSynthesizer_MlDisplayFunction() override;
        MlGrammarSynthesizer_MlDisplayFunction(MlGrammarSynthesizer_MlDisplayFunction&) = delete;
        MlGrammarSynthesizer_MlDisplayFunction& operator=(MlGrammarSynthesizer_MlDisplayFunction&) = delete;

    private:
        friend class MlGrammarSynthesizer;
    };
} // namespace inflection::grammar::synthesis