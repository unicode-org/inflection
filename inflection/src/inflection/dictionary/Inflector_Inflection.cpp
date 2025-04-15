/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/Inflector_Inflection.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <string>

namespace inflection::dictionary {

Inflector_Inflection::Inflector_Inflection(
        const Inflector_InflectionPattern& inflectionPattern, int32_t suffixId,
        int64_t grammemes)
    : inflectionPattern(&inflectionPattern)
    , suffixId(suffixId)
    , grammemes(grammemes)
{
}

::std::u16string Inflector_Inflection::inflect(const ::std::u16string& lemma) const
{
    std::u16string suffix = getSuffix();
    const auto& parent = *npc(inflectionPattern);

    for (int16_t i = 0; i < parent.lemmaSuffixesLen; ++i) {
        std::u16string lemmaSuffix(parent.inflectorDictionary.inflectionSuffixes.getString(
                parent.inflectorDictionary.inflectionsArray.read(parent.lemmaSuffixesOffset + i)));
        if (lemma.ends_with(lemmaSuffix)) {
            return lemma.substr(0, lemma.size() - lemmaSuffix.size()) + suffix;
        }
    }

    return lemma + suffix;
}

::std::u16string Inflector_Inflection::lemmatize(const ::std::u16string& noun) const
{
    std::u16string suffix = getSuffix();
    const auto& parent = *npc(inflectionPattern);

    if (parent.lemmaSuffixesLen != 0 && noun.ends_with(suffix)) {
        return noun.substr(0, noun.size() - suffix.size())
                + parent.inflectorDictionary.inflectionSuffixes.getString(
                    parent.inflectorDictionary.inflectionsArray.read(parent.lemmaSuffixesOffset));
    }

    return noun;
}

::std::u16string Inflector_Inflection::getSuffix() const
{
    return npc(inflectionPattern)->inflectorDictionary.inflectionSuffixes.getString(suffixId);
}

int64_t Inflector_Inflection::getGrammemes() const {
    return grammemes;
}

const inflection::dictionary::Inflector_InflectionPattern& Inflector_Inflection::getInflectionPattern() const {
    return *npc(inflectionPattern);
}

} // namespace inflection::dictionary
