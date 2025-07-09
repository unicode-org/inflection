/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/lang/features/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <string>
#include <set>

/**
 * @brief A container of grammemes of a specific grammatical category.
 * @details
 * A grammatical category are concepts that involve selecting a variant of a word or are properties of a word.
 * A category like number is represented as count in Siri. The grammeme values are singular and plural in English.
 * In some other languages, this concept may not exist. In another language, there may several plural forms
 * to choose from.
 *
 * @see <a href="http://en.wikipedia.org/wiki/Grammatical_category">http://en.wikipedia.org/wiki/Grammatical_category</a>
 */
class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures_GrammarCategory
    : public virtual ::inflection::Object
{
public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

private:
    std::u16string name {  };
    std::set<std::u16string> values {  };
    std::multimap<std::u16string, std::u16string> grammemeDependenciesForCategory {  };
    std::map<std::u16string_view, std::multimap<std::u16string, std::u16string>> grammemeDependenciesForValues {  };
    bool uniqueValues { true };

public:
    /**
     * The name of the grammatical category. For example, it can be {@code pos} for part of speech or gender.
     */
    std::u16string getName() const;
    /**
     * These are all of the possible values for the given grammatical category. These are typically called grammemes.
     */
    std::set<std::u16string> getValues() const;
    /**
     * Returns true when the values are unique considering all grammatical categories of a given language.
     */
    bool isUniqueValues() const;
    /**
     * Returns a map of category names with their category values (grammemes) that this category depends on.
     * For example, verbs and nouns are affected by different grammatical categories.
     * Verbs have grammatical tense and nouns do not have grammatical tense.
     */
    std::multimap<std::u16string, std::u16string> getGrammemeDependenciesForCategory() const;
    /**
     * Returns a map of category names with their category values (grammemes) that the specified grammeme value depends on.
     * For example, grammatical gender may have a different set of grammemes valid between nouns and pronouns.
     * @param grammemeValue Use <code>getValues()</code> to get the list of values that can be used as an argument.
     */
    std::multimap<std::u16string, std::u16string> getGrammemeDependenciesForValue(std::u16string_view grammemeValue) const;

    /**
     * String compares the order of the name.
     * @param other The object to be compared with this.
     */
    std::weak_ordering operator<=>(const LanguageGrammarFeatures_GrammarCategory& other) const;

private: /* package */
    LanguageGrammarFeatures_GrammarCategory(const std::u16string& name, const std::set<std::u16string>& values,
                                            const std::multimap<std::u16string, std::u16string>& grammemeDependenciesForCategory,
                                            const std::map<std::u16string_view, std::multimap<std::u16string, std::u16string>>& grammemeDependenciesForValues);
public:
    /**
     * Destructor
     */
    ~LanguageGrammarFeatures_GrammarCategory() override;

private:
    friend class LanguageGrammarFeatures;
};
