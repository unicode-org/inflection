/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/DictionaryKeyIterator.hpp>

#include <morphuntion/dictionary/metadata/MarisaTrieIterator.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dictionary {

DictionaryKeyIterator::DictionaryKeyIterator(const metadata::MarisaTrieIterator<uint64_t>& trieIterator)
    : trieIterator(new metadata::MarisaTrieIterator<uint64_t>(trieIterator.trie, trieIterator.prefix))
{
    // Skip the empty string
    if (operator*().empty() && trieIterator.prefix.empty()) {
        operator++();
    }
}

DictionaryKeyIterator::DictionaryKeyIterator(DictionaryKeyIterator&& iterator)
    : trieIterator(iterator.trieIterator)
{
    iterator.trieIterator = nullptr;
}

DictionaryKeyIterator::~DictionaryKeyIterator()
{
    delete trieIterator;
}

const ::std::u16string&
DictionaryKeyIterator::operator*() const {
    return (*(*npc(trieIterator))).first;
}

DictionaryKeyIterator&
DictionaryKeyIterator::operator++() {
    ++*npc(trieIterator);
    return *this;
}

bool
DictionaryKeyIterator::operator==(const DictionaryKeyIterator& rhs) const {
    return *trieIterator == *rhs.trieIterator;
}

bool
DictionaryKeyIterator::operator!=(const DictionaryKeyIterator& rhs) const {
    return !(*this == rhs);
}

DictionaryKeyIterator
DictionaryKeyIterator::begin() const
{
    return DictionaryKeyIterator(npc(trieIterator)->begin());
}

DictionaryKeyIterator
DictionaryKeyIterator::end() const
{
    return DictionaryKeyIterator(npc(trieIterator)->end());
}

} // namespace morphuntion::dictionary
