/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/lang/features/fwd.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp>
#include <morphuntion/Object.hpp>
#include <string>
#include <map>

/**
 * @brief A single feature value with a set of constraints for when the value is valid.
 * @details
 * A grammatical feature is a specific word used to be in grammatical agreement with semantic concepts given a set of constraints.
 * Frequently they are articles, prepositions and measure words. These values are typically based (constrained)
 * on the grammatical category values of the nouns.
 */
class MORPHUNTION_CLASS_API morphuntion::lang::features::LanguageGrammarFeatures_Feature
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

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
