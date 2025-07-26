/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/Object.hpp>
#include <inflection/util/ULocale.hpp>
#include <map>

class inflection::dialog::DefinitenessDisplayFunction
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private:
    const ::inflection::dialog::SemanticFeature* definiteFeature { nullptr };
    const ::inflection::dialog::SemanticFeature* speakFeature { nullptr };
    const ::inflection::dialog::DefaultFeatureFunction* definiteFeatureFunction { nullptr };
    const ::inflection::dialog::DefaultFeatureFunction* indefiniteFeatureFunction { nullptr };
    std::set<std::u16string, std::less<>> definiteArticles {  };
    std::set<std::u16string, std::less<>> indefiniteArticles {  };

private:
    static std::set<std::u16string, std::less<>> getArticles(const inflection::util::ULocale& locale, std::u16string_view featureName);
    static int32_t getArticlePrefixLength(DisplayValue* originalDisplayValue, const std::set<std::u16string, std::less<>>& articlesToRemove);
    DisplayValue* replaceDisplayValue(DisplayValue* originalDisplayValue, const SpeakableString& string) const;

public:
    virtual DisplayValue* addDefiniteness(DisplayValue* displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;
    virtual DisplayValue* updateDefiniteness(DisplayValue* displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public:
    DefinitenessDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::inflection::dialog::DefaultFeatureFunction* definiteFeatureFunction, std::u16string_view definiteFeatureArticlesName, const ::inflection::dialog::DefaultFeatureFunction* indefiniteFeatureFunction, std::u16string_view indefiniteFeatureArticlesName);
    DefinitenessDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& definiteFeatureName, std::u16string_view definiteFeatureArticlesName, const ::std::u16string& indefiniteFeatureName, std::u16string_view indefiniteFeatureArticlesName);
    ~DefinitenessDisplayFunction() override;
    DefinitenessDisplayFunction(const DefinitenessDisplayFunction&) = delete;
    DefinitenessDisplayFunction& operator=(const DefinitenessDisplayFunction&) = delete;
};
