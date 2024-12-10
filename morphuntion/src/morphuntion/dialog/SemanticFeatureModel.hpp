/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticValue.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

/**
 * @brief A semantic model that contains display data for semantic values and provides feature functions for deriving missing data.
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SemanticFeatureModel final
    : public ::morphuntion::Object
{

public:
    typedef ::morphuntion::Object super;

private:
    ::std::map<::std::u16string, ::morphuntion::dialog::SemanticFeature*> featureMap {  };
    ::std::map<::std::u16string, ::std::pair<::morphuntion::dialog::SemanticFeature*, ::std::u16string>> featuresAliases {  };
    ::std::map<SemanticValue, SemanticFeatureModel_DisplayData> semanticValueMap {  };
    ::std::map<::morphuntion::dialog::SemanticFeature, DefaultFeatureFunction*> defaultFeatureFunctions {  };
    ::morphuntion::util::ULocale locale;
    DefaultDisplayFunction* defaultDisplayFunction {  };

public:
    static constexpr auto SPEAK = u"speak";

    /**
     * Returns all of the semantic values are associated with SemanticFeatureModel_DisplayData in this SemanticFeatureModel
     */
    ::std::set<morphuntion::dialog::SemanticValue> getKnownSemanticValues() const;
    /**
     * Returns the SemanticFeatureModel_DisplayData associated with the semantic value in this SemanticFeatureModel
     */
    const SemanticFeatureModel_DisplayData* getDisplayData(const SemanticValue& semantic) const;
    const SemanticFeature* getFeature(const ::std::u16string& name) const;
    ::std::pair<::morphuntion::dialog::SemanticFeature*, ::std::u16string> getFeatureAlias(const ::std::u16string& name) const;
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
    void putDefaultFeatureFunctionByName(const ::std::u16string& feature, DefaultFeatureFunction* function);
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
    static ::std::vector<::morphuntion::dialog::SemanticFeature*> createDefaultSemanticFeatures(const ::morphuntion::util::ULocale& locale);

public:
    /**
     * Creates a default SemanticFeatureModel for a given locale without any display data.
     */
    explicit SemanticFeatureModel(const ::morphuntion::util::ULocale& locale);
    SemanticFeatureModel(const ::morphuntion::util::ULocale& locale, const ::std::map<SemanticValue, SemanticFeatureModel_DisplayData>& semanticValueMap);
    /**
     * Destructor
     */
    ~SemanticFeatureModel() override;
private:
    SemanticFeatureModel(const SemanticFeatureModel& other) = delete;
    SemanticFeatureModel& operator=(const SemanticFeatureModel&) = delete;
};
