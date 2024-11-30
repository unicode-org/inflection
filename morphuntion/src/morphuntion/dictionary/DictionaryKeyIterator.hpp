/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/metadata/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <cstdint>
#include <string>

/**
 * @brief Provides the ability to iterate over the words from a @ref DictionaryMetaData object.
 */
class MORPHUNTION_CLASS_API morphuntion::dictionary::DictionaryKeyIterator
    : public virtual ::morphuntion::Object
{

public:
    typedef ::morphuntion::Object super;

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
     * Return false when this is equal to the other iterator.
     */
    bool operator!=(const DictionaryKeyIterator& rhs) const;

    /**
     * The first word in the dictionary.
     */
    DictionaryKeyIterator    begin() const;
    /**
     * The last word in the dictionary.
     */
    DictionaryKeyIterator    end() const;
private:

    explicit DictionaryKeyIterator(const ::morphuntion::dictionary::metadata::MarisaTrieIterator<uint64_t>& trieIterator);

    ::morphuntion::dictionary::metadata::MarisaTrieIterator<uint64_t>* trieIterator {  };

    friend class DictionaryMetaData_MMappedDictionary;
};
