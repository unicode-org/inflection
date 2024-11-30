/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <map>

class morphuntion::dialog::DefinitenessDisplayFunction
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

private:
    const ::morphuntion::dialog::SemanticFeature* definiteFeature { nullptr };
    const ::morphuntion::dialog::SemanticFeature* speakFeature { nullptr };
    const ::morphuntion::dialog::DefaultFeatureFunction* definiteFeatureFunction { nullptr };
    const ::morphuntion::dialog::DefaultFeatureFunction* indefiniteFeatureFunction { nullptr };
    std::set<std::u16string> definiteArticles {  };
    std::set<std::u16string> indefiniteArticles {  };

private:
    static std::set<std::u16string> getArticles(const morphuntion::util::ULocale& locale, std::u16string_view featureName);
    static int32_t getArticlePrefixLength(SemanticFeatureModel_DisplayValue* originalDisplayValue, const std::set<std::u16string>& articlesToRemove);
    SemanticFeatureModel_DisplayValue* replaceDisplayValue(SemanticFeatureModel_DisplayValue* originalDisplayValue, const SpeakableString& string) const;

public:
    virtual SemanticFeatureModel_DisplayValue* addDefiniteness(SemanticFeatureModel_DisplayValue* displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const;

public:
    DefinitenessDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::morphuntion::dialog::DefaultFeatureFunction* definiteFeatureFunction, std::u16string_view definiteFeatureArticlesName, const ::morphuntion::dialog::DefaultFeatureFunction* indefiniteFeatureFunction, std::u16string_view indefiniteFeatureArticlesName);
    DefinitenessDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& definiteFeatureName, std::u16string_view definiteFeatureArticlesName, const ::std::u16string& indefiniteFeatureName, std::u16string_view indefiniteFeatureArticlesName);
    ~DefinitenessDisplayFunction() override;
};
