/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/Object.hpp>
#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief Provides the ability to iterate over the words from a @ref DictionaryMetaData object.
 */
class INFLECTION_CLASS_API inflection::dictionary::DictionaryKeyIterator
    : public virtual ::inflection::Object
{

public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

    /**
     * Move constructor
     */
    DictionaryKeyIterator(DictionaryKeyIterator&& iterator);
    /**
     * Destructor
     */
    ~DictionaryKeyIterator() override;

    /**
     * Retrieve a reference to the current word.
     */
    const ::std::u16string& operator*() const;

    /**
     * Iterate to the next word
     */
    DictionaryKeyIterator&  operator++();

    /**
     * Return true when this is equal to the other iterator.
     */
    bool operator==(const DictionaryKeyIterator& rhs) const;

    /**
     * The first word in the dictionary.
     */
    DictionaryKeyIterator    begin() const;
    /**
     * The last word in the dictionary.
     */
    DictionaryKeyIterator    end() const;

    DictionaryKeyIterator(const DictionaryKeyIterator&) = delete;
    DictionaryKeyIterator& operator=(const DictionaryKeyIterator&) = delete;
private:

    explicit DictionaryKeyIterator(const ::inflection::dictionary::metadata::MarisaTrieIterator<uint64_t>& trieIterator);

    std::unique_ptr<::inflection::dictionary::metadata::MarisaTrieIterator<uint64_t>> trieIterator {  };

    friend class DictionaryMetaData_MMappedDictionary;
};
