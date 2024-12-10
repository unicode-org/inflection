/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/lang/features/fwd.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_Feature.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarFeatures.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <set>
#include <string>

/**
 * @brief This class provides information about semantic features and grammatical categories
 * used for the classes derived from SemanticFeatureConceptBase for various languages.
 *
 * @author George Rhoten
 */
class MORPHUNTION_CLASS_API morphuntion::lang::features::LanguageGrammarFeatures
    : public virtual ::morphuntion::Object
{

public:
    typedef ::morphuntion::Object super;

private:
    ::morphuntion::util::ULocale language;
    const Grammar& grammar;
    const Features& features;

public:
    /**
     * Return the LanguageGrammarFeatures as specified for the locale.
     * @exception IllegalArgumentException is thrown if an unknown locale is requested.
     */
    static ::morphuntion::lang::features::LanguageGrammarFeatures getLanguageGrammarFeatures(const ::morphuntion::util::ULocale& locale);
    /**
     * Returns a set of locales that are known for querying LanguageGrammarFeatures.
     */
    static std::set<::morphuntion::util::ULocale> getKnownLanguages();
    /**
     * Returns the resolved language for this object.
     */
    ::morphuntion::util::ULocale getLanguage() const;
    /**
     * Returns the total number of categories. This can be helpful if you don't need a copy of all of the categories.
     */
    int32_t getCategoriesCount() const;
private:
    static void addCategory(std::map<std::u16string, LanguageGrammarFeatures_GrammarCategory>* categories, const Category& category);
public:
    /**
     * Returns a map of category names to GrammarCategory objects.
     */
    std::map<std::u16string, ::morphuntion::lang::features::LanguageGrammarFeatures_GrammarCategory> getCategories() const;
    /**
     * Returns the total number of features. This can be helpful if you don't need a copy of all of the features.
     */
    int32_t getFeaturesCount() const;
    /**
     * Returns a map of grammar feature names to GrammarFeatures objects.
     */
    std::set<::morphuntion::lang::features::LanguageGrammarFeatures_GrammarFeatures> getFeatures() const;

private:
    explicit LanguageGrammarFeatures(const ::morphuntion::util::ULocale& language, const Grammar& grammar, const Features& features);
public:
    /**
     * Destructor
     */
    ~LanguageGrammarFeatures() override;

private:
    friend class LanguageGrammarFeatures_GrammarCategory;
    friend class LanguageGrammarFeatures_Feature;
    friend class LanguageGrammarFeatures_GrammarFeatures;
};
