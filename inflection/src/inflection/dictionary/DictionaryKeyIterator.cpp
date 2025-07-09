/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryKeyIterator.hpp>

#include <inflection/dictionary/metadata/MarisaTrieIterator.hpp>
#include <inflection/npc.hpp>

namespace inflection::dictionary {

DictionaryKeyIterator::DictionaryKeyIterator(const metadata::MarisaTrieIterator<uint64_t>& trieIterator)
    : trieIterator(new metadata::MarisaTrieIterator<uint64_t>(trieIterator.trie, trieIterator.prefix))
{
    // Skip the empty string
    if (operator*().empty() && trieIterator.prefix.empty()) {
        operator++();
    }
}

DictionaryKeyIterator::DictionaryKeyIterator(DictionaryKeyIterator&& iterator)
    : trieIterator(npc(iterator.trieIterator.release()))
{
}

DictionaryKeyIterator::~DictionaryKeyIterator()
{
}

const ::std::u16string&
DictionaryKeyIterator::operator*() const {
    return (*(*trieIterator)).first;
}

DictionaryKeyIterator&
DictionaryKeyIterator::operator++() {
    ++*trieIterator;
    return *this;
}

bool
DictionaryKeyIterator::operator==(const DictionaryKeyIterator& rhs) const {
    return *trieIterator == *rhs.trieIterator;
}

DictionaryKeyIterator
DictionaryKeyIterator::begin() const
{
    return DictionaryKeyIterator(trieIterator->begin());
}

DictionaryKeyIterator
DictionaryKeyIterator::end() const
{
    return DictionaryKeyIterator(trieIterator->end());
}

} // namespace inflection::dictionary
