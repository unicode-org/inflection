/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/SemanticValue.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

/**
 * @brief A semantic model that contains display data for semantic values and provides feature functions for deriving missing data.
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureModel final
    : public ::inflection::Object
{

public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

private:
    ::std::map<::std::u16string, ::inflection::dialog::SemanticFeature*, std::less<>> featureMap {  };
    ::std::map<::std::u16string, ::std::pair<::inflection::dialog::SemanticFeature*, ::std::u16string>, std::less<>> featuresAliases {  };
    ::std::map<SemanticValue, SemanticFeatureModel_DisplayData> semanticValueMap {  };
    ::std::map<::inflection::dialog::SemanticFeature, DefaultFeatureFunction*> defaultFeatureFunctions {  };
    ::inflection::util::ULocale locale;
    std::unique_ptr<DefaultDisplayFunction> defaultDisplayFunction;

public:
    static constexpr auto SPEAK = u"speak";

    /**
     * Returns all of the semantic values are associated with SemanticFeatureModel_DisplayData in this SemanticFeatureModel
     */
    ::std::set<inflection::dialog::SemanticValue> getKnownSemanticValues() const;
    /**
     * Returns the SemanticFeatureModel_DisplayData associated with the semantic value in this SemanticFeatureModel
     */
    const SemanticFeatureModel_DisplayData* getDisplayData(const SemanticValue& semantic) const;
    const SemanticFeature* getFeature(::std::u16string_view name) const;
    ::std::pair<::inflection::dialog::SemanticFeature*, ::std::u16string> getFeatureAlias(::std::u16string_view name) const;
    /**
     * Returns the locale created with this SemanticFeatureModel.
     */
    const util::ULocale& getLocale() const;
    const DefaultFeatureFunction* getDefaultFeatureFunction(const SemanticFeature& feature) const;
    void putDefaultFeatureFunction(const SemanticFeature& feature, DefaultFeatureFunction* function);
    /**
     * This function works the same way as putDefaultFeatureFunction, but an exception is thrown if the feature is unknown.
     * @param feature The name of the SemanticFeature in this SemanticFeatureModel.
     * @param function The function to derive the specified SemanticFeature for the locale when it's not specified by available SemanticFeatureModel_DisplayData.
     */
    void putDefaultFeatureFunctionByName(::std::u16string_view feature, DefaultFeatureFunction* function);
    /**
     * Returns the function is used for inflecting phrases when no constraints match the available SemanticFeatureModel_DisplayData.
     * When it's null, then there is no way to inflect phrases from the constraints.
     * @see getDisplayData
     */
    const DefaultDisplayFunction* getDefaultDisplayFunction() const;
    /**
     * This function is used for inflecting phrases when no constraints match the available SemanticFeatureModel_DisplayData.
     * Normally this does not need to be set.
     * @see getDisplayData
     */
    void setDefaultDisplayFunction(DefaultDisplayFunction* defaultDisplayFunction);

private:
    static ::std::vector<::inflection::dialog::SemanticFeature*> createDefaultSemanticFeatures(const ::inflection::util::ULocale& locale);

public:
    /**
     * Creates a default SemanticFeatureModel for a given locale without any display data.
     */
    explicit SemanticFeatureModel(const ::inflection::util::ULocale& locale);
    SemanticFeatureModel(const ::inflection::util::ULocale& locale, const ::std::map<SemanticValue, SemanticFeatureModel_DisplayData>& semanticValueMap);
    /**
     * Destructor
     */
    ~SemanticFeatureModel() override;
private:
    SemanticFeatureModel(const SemanticFeatureModel& other) = delete;
    SemanticFeatureModel& operator=(const SemanticFeatureModel&) = delete;
};
