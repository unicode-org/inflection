/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/lang/features/fwd.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <inflection/Object.hpp>
#include <string>
#include <map>

/**
 * @brief A single feature value with a set of constraints for when the value is valid.
 * @details
 * A grammatical feature is a specific word used to be in grammatical agreement with semantic concepts given a set of constraints.
 * Frequently they are articles, prepositions and measure words. These values are typically based (constrained)
 * on the grammatical category values of the nouns.
 */
class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures_Feature
    : public virtual ::inflection::Object
{
public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

private:
    std::u16string value {  };
    std::map<std::u16string, std::u16string> constraints {  };

public:
    /**
     * The value to use given the constraints.
     */
    virtual std::u16string getValue() const;
    /**
     * The constraints of grammatical categories and their values to choose this value.
     */
    virtual std::map<std::u16string, std::u16string> getConstraints() const;

public: /* package */
    /**
     * Construct a feature by value and a mapping of constraints.
     * @param value the name of the feature.
     * @param constraints The constraints that are valid for the value. This map contains 
     *        grammatical category name  mapped to the specific value valid for the 
     *        grammatical category.
     */
    LanguageGrammarFeatures_Feature(const std::u16string& value, const std::map<std::u16string, std::u16string>& constraints);
    /**
     * Destructor
     */
    ~LanguageGrammarFeatures_Feature() override;

private:
    friend class LanguageGrammarFeatures;
    friend class LanguageGrammarFeatures_GrammarCategory;
    friend class LanguageGrammarFeatures_GrammarFeatures;
};
