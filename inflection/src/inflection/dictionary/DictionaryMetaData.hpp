/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/DictionaryKeyIterator.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <string_view>

/**
 * @brief Provides an interface to read Inflection's lexical dictionaries.
 */
class INFLECTION_CLASS_API inflection::dictionary::DictionaryMetaData
    : public virtual ::inflection::Object
{

public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

public:
    /**
     * Factory method to return a DictionaryMetaData singleton for each locale.
     * @param locale The locale to get the lexical dictionary.
     * @return A singleton. Do not delete this object.
     */
    static const DictionaryMetaData* createDictionary(const ::inflection::util::ULocale& locale);

private:
    static ::std::u16string* transform(::std::u16string* dest, std::u16string_view str, const ::inflection::util::ULocale& locale);

public:
    /**
     * Get all of the binary types of a word or phrase. The name for each bit can be retrieved by getPropertyName().
     */
    int64_t* getCombinedBinaryType(int64_t* result, std::u16string_view word) const;

    /**
     * Turns all of the bits in the binary properties into string based property names.
     */
    ::std::vector<::std::u16string> getPropertyNames(int64_t binaryProperties) const;
    /**
     * Given a single bit turned on, return the name of the property.
     */
    ::std::u16string getPropertyName(int64_t singleProperty) const;
    /**
     * Is the provided word a known word in the dictionary.
     */
    bool isKnownWord(std::u16string_view word) const;
    /**
     * Does the given word or phrase have the specified property.
     * If you plan to do a lot of these operations, it's faster to turn the property name into a binary
     * representation with getBinaryProperties(), and then compare the binary properties with the result
     * from getCombinedBinaryType() for the word or phrase.
     */
    bool hasProperty(std::u16string_view word, std::u16string_view partOfSpeech) const;
    /**
     * Does the given word or phrase have all of the specified properties.
     */
    bool hasAllProperties(std::u16string_view word, int64_t partOfSpeech) const;
    /**
     * Does the given word or phrase have at least one of the specified properties.
     */
    bool hasAnyProperty(std::u16string_view word, int64_t partOfSpeech) const;
    /**
     * Given the string form of the property names, return a bit field representing each requested property
     * @param result If the properties are known, the result value is set with the binary value representing the requested properties.
     * @param properties These are the string form of the word properties
     * @return null if no properties are found.
     */
    int64_t* getBinaryProperties(int64_t* result, const ::std::vector<::std::u16string>& properties) const;
    /**
     * Retrieve all of the property names and return set the provided return value.
     * @return The binary value representing the requested properties.
     */
    int64_t getBinaryProperties(const ::std::vector<::std::u16string>& properties) const;
    /**
     * Returns the string form of all of the binary property values.
     */
    ::std::vector<::std::u16string> getProperties(std::u16string_view word) const;
    /**
     * Returns the all of the values for the given property key.
     */
    ::std::vector<::std::u16string> getPropertyValues(std::u16string_view word, std::u16string_view partOfSpeech) const;
    /**
     * Returns the all of the values for the given property key.
     */
    bool getPropertyValues(::std::vector<::std::u16string>* result, std::u16string_view word, std::u16string_view partOfSpeech) const;
    /**
     * Returns an iterator to iterate over all known words in this dictionary.
     */
    ::inflection::dictionary::DictionaryKeyIterator getKnownWords() const;
    /**
     * Returns the number of known words in this dictionary.
     */
    int32_t getKnownWordsSize() const;

private:
    explicit DictionaryMetaData(DictionaryMetaData_MMappedDictionary* dictionary);
    DictionaryMetaData(const DictionaryMetaData& other) = delete;
    DictionaryMetaData& operator=(const DictionaryMetaData& other) = delete;
    ~DictionaryMetaData() override;

private:
    std::unique_ptr<DictionaryMetaData_MMappedDictionary> dictionary;
private:
    friend class Inflector;
    friend class Inflector_MMappedDictionary;
};
