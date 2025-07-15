/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <marisa/trie.h>
#include <tuple>
#include <string_view>

template <typename T>
class inflection::dictionary::metadata::MarisaTrieIterator
{

public:
    explicit MarisaTrieIterator(const MarisaTrie<T>& trie, std::u16string_view prefix)
        : trie(trie)
        , prefix(prefix)
    {
        ::std::string searchPrefix;
        if (!prefix.empty()) {
            trie.encoder.encode(&searchPrefix, prefix);
        }
        agent.set_query(searchPrefix.c_str());
        if (prefix.length() == 1 && prefix.at(0) == u'\uFFFF') {
            // Special way to set the internal state of an end iterator by using not a Unicode character.
            reachedEnd = true;
        }
        else {
            operator++();
        }
        // else it's an end iterator
    }
    MarisaTrieIterator(MarisaTrieIterator<T>&& iterator)
        : trie(iterator.trie)
        , prefix(iterator.prefix)
        , value(iterator.value)
        , reachedEnd(iterator.reachedEnd)
    {
        agent.swap(iterator.agent);
    }
    ~MarisaTrieIterator() {}

    const ::std::pair<::std::u16string, T>& operator*();

    MarisaTrieIterator<T>&   operator++();

    bool operator==(const MarisaTrieIterator<T>& rhs) const;

    MarisaTrieIterator<T>    begin() const;
    MarisaTrieIterator<T>    end() const;
private:

    const ::inflection::dictionary::metadata::MarisaTrie<T>& trie;
    ::marisa::Agent agent {  };
    ::std::u16string prefix {  };
    ::std::pair<::std::u16string, T> value {  };
    bool reachedEnd { false };

    friend class inflection::dictionary::DictionaryKeyIterator;
};

template <typename T>
const ::std::pair<::std::u16string, T>&
inflection::dictionary::metadata::MarisaTrieIterator<T>::operator*() {
    if (reachedEnd) {
        value = {u"", {}};
    }
    else {
        trie.encoder.decode(&value.first, agent.key().ptr(), int32_t(agent.key().length()));
        value.second = trie.data.read((int32_t) agent.key().id());
    }
    return value;
}

template <typename T>
::inflection::dictionary::metadata::MarisaTrieIterator<T>&
inflection::dictionary::metadata::MarisaTrieIterator<T>::operator++() {
    if (!trie.trie.predictive_search(agent)) {
        reachedEnd = true;
    }
    return *this;
}

template <typename T>
bool
inflection::dictionary::metadata::MarisaTrieIterator<T>::operator==(const MarisaTrieIterator& rhs) const {
    // Same trie with the same key id. It has to be the same.
    return &trie == &rhs.trie && reachedEnd == rhs.reachedEnd && (reachedEnd || agent.key().id() == rhs.agent.key().id());
}

template <typename T>
inflection::dictionary::metadata::MarisaTrieIterator<T>
inflection::dictionary::metadata::MarisaTrieIterator<T>::begin() const
{
    return MarisaTrieIterator<T>(trie, prefix);
}

template <typename T>
inflection::dictionary::metadata::MarisaTrieIterator<T>
inflection::dictionary::metadata::MarisaTrieIterator<T>::end() const
{
    return MarisaTrieIterator<T>(trie, u"\uFFFF");
}

