/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <vector>

/**
 * @brief Facilitates the creation of _Concepts_
 * @details Concepts can generally be defined as a way to lexically represent real-world or linguistic ideas. These ideas can
 * vary greatly in scope, and range from the representation of a mathematical number by a language, to rules on how to
 * properly affix a suffix to a token in order to represent plurality.
 *
 * You should never instantiate this interface yourself. Instead use CommonConceptFactoryProvider to get an instance
 * of this class for any supported locale.
 *
 * To use this class, it is recommended to use LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider() to
 * get a LocalizedCommonConceptFactoryProvider singleton, then use
 * CommonConceptFactoryProvider::getCommonConceptFactory() to get a CommonConceptFactory in any supported locale:
 * @code
 * #include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
 * #include <morphuntion/dialog/CommonConceptFactory.hpp>
 *
 * auto commonConceptFactory =
 *      morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()->getCommonConceptFactory(morphuntion::util::LocaleUtils::PORTUGUESE())
 * @endcode
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::CommonConceptFactory
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

public:
    /**
     * Returns a pointer to the inheriting ConceptFactory's SemanticFeatureModel.
     *
     * @return The underlying language's SemanticFeatureModel
     */
    virtual const SemanticFeatureModel* getSemanticFeatureModel() const = 0;

    /**
     * Returns this ConceptFactory's underlying language.
     *
     * @return The language.
     */
    virtual const ::morphuntion::util::ULocale& getLanguage() const = 0;

    /**
     * Returns a new InflectableStringConcept based on an input SpeakableString.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new InflectableStringConcept.
     * @param string SpeakableString used to initialize the concept with a value to inflect upon.
     */
    virtual InflectableStringConcept* createConcept(const ::morphuntion::dialog::SpeakableString& string) const = 0;

    /**
     * Returns a new SemanticConceptList based on an input <code>std::vector</code> of _concepts_.
     * The returned list is created using the semantically optional specifier (i.e. "or" in English).
     *
     * An example of using this function would be creating an array of concepts using the input strings
     * <code>["chocolate", "vanilla", "caramel"]</code> and the resulting output would be
     * <code>"chocolate, vanilla, or caramel"</code>.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SemanticConceptList.
     * @param concepts std::vector of semantic feature concepts. These should be subclasses of
     * SemanticFeatureConceptBase.
     */
    virtual SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const = 0;

    /**
     * Returns a new SemanticConceptList based on an input <code>std::vector</code> of _concepts_.
     * The returned list is created using the semantically optional specifier (i.e. "or" in English).
     *
     * An example of using this function would be creating an array of concepts using the input strings
     * <code>["chocolate", "vanilla", "caramel"]</code> and the resulting output would be
     * <code>"chocolate, vanilla, or caramel"</code>.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SemanticConceptList.
     * @param concepts std::vector of semantic feature concepts. These should be subclasses of
     * SemanticFeatureConceptBase.
     */
    virtual SemanticConceptList* createOrList(const ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const = 0;

    /**
     * Returns a new SemanticConceptList based on an input <code>std::vector</code> of _concepts_.
     * The returned list is created using the semantically inclusive specifier (i.e. "and" in English).
     *
     * An example of using this function would be creating an array of concepts using the input strings
     * <code>["chocolate", "vanilla", "caramel"]</code> and the resulting output would be
     * <code>"chocolate, vanilla, and caramel"</code>.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SemanticConceptList.
     * @param concepts std::vector of semantic feature concepts. These should be subclasses of
     * SemanticFeatureConceptBase.
     */
    virtual SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const = 0;

    /**
     * Returns a new SemanticConceptList based on an input <code>std::vector</code> of _concepts_.
     * The returned list is created using the semantically inclusive specifier (i.e. "and" in English).
     *
     * An example of using this function would be creating an array of concepts using the input strings
     * <code>["chocolate", "vanilla", "caramel"]</code> and the resulting output would be
     * <code>"chocolate, vanilla, and caramel"</code>.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SemanticConceptList.
     * @param concepts std::vector of semantic feature concepts. These should be subclasses of
     * SemanticFeatureConceptBase.
     */
    virtual SemanticConceptList* createAndList(const ::std::vector<::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const = 0;

    /**
     * Returns a new NumberConcept based on this concept factory's language, the region format for printing and <code>int64_t</code> number.
     *
     * The returned object is owned by the caller and must be deleted by the caller.
     *
     * @param regionFormatLocale The locale for printing the number. The language will be used for the speak value.
     * @param number The input number.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* number(const ::morphuntion::util::ULocale& regionFormatLocale, int64_t number) const = 0;

    /**
     * Returns a new NumberConcept based on this concept factory's language, the region format for printing and <code>double</code> number.
     * Returns a new NumberConcept based on a specified RequestContext and <code>double</code> number.
     *
     * The returned object is owned by the caller and must be deleted by the caller.
     *
     * @param regionFormatLocale The locale for printing the number. The language will be used for the speak value.
     * @param number The input number.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* number(const ::morphuntion::util::ULocale& regionFormatLocale, double number) const = 0;

    /**
     * Returns a SpeakableString based on an input SpeakableString and with the
     * print component surrounded with the locale's quotation punctuation, in English this is represented with
     * quotation marks: <code>""</code>. Another example is in Swiss German dialects this is represented with
     * <code>«»</code>.
     *
     * @return The quoted SpeakableString.
     * @param str The input SpeakableString to add quotation punctuation to.
     */
    virtual SpeakableString quote(const ::morphuntion::dialog::SpeakableString& str) const = 0;

    /**
     * Returns a new SpeakableString based on a NumberConcept and SemanticConcept.
     *
     * Usage for this would be to automatically determine plurality rules for words. This is especially
     * useful for words that have multiple rules based on amounts, such as Slavic languages, in which the genetive
     * plural form has a different inflection.
     *
     * For example in Russian:<br/>
     * 0 years → 0 лет<br/>
     * 1 year → 1 год<br/>
     * 4 years → 4 года<br/>
     * 5 years → 5 лет<br/>
     * 100 years → 100 лет
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SpeakableString.
     * @param number An input NumberConcept to represent the amount.
     * @param semanticConcept An input SemanticConcept to represent the word to be inflected based on
     * the amount.
     */
    virtual SpeakableString* quantify(const NumberConcept& number, const ::morphuntion::dialog::SemanticFeatureConceptBase* semanticConcept) const = 0;

    /**
     * Returns a new SpeakableString based on a NumberConcept, format number, and SemanticConcept.
     *
     * The returned pointer is _unmanaged_ and must be manually **deleted**.
     *
     * @return A new SpeakableString.
     * @param number An input NumberConcept to represent the amount.
     * @param formattedNumber If the default chosen cardinal number is wrong for the context of the noun, you can format a specific form of the number.
     * @param semanticConcept An input SemanticConcept to represent the word to be inflected based on
     * the amount.
     */
    virtual SpeakableString* quantify(const NumberConcept& number, const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SemanticFeatureConceptBase* semanticConcept) const = 0;

    /**
     * Destructor
     *
     * This destructor does **NOT** delete created concepts. Created concepts must be manually **deleted**.
     */
    ~CommonConceptFactory() override;
};
