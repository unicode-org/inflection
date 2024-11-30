/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/Inflector_Inflection.hpp>
#include <morphuntion/dictionary/Inflector_InflectionPattern.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <string>

namespace morphuntion::dictionary {

Inflector_Inflection::Inflector_Inflection(
        const Inflector_InflectionPattern& inflectionPattern, int32_t suffixId,
        int64_t grammemes)
    : inflectionPattern(inflectionPattern)
    , suffixId(suffixId)
    , grammemes(grammemes)
{
}

::std::u16string Inflector_Inflection::inflect(const ::std::u16string& lemma) const
{
    std::u16string suffix = getSuffix();

    for(int16_t i = 0; i < inflectionPattern.lemmaSuffixesLen; ++i) {
        std::u16string lemmaSuffix(inflectionPattern.suffixes.getString(
                inflectionPattern.inflectorDictionary.inflectionsArray.read(inflectionPattern.lemmaSuffixesOffset + i)));
        if (::morphuntion::util::StringViewUtils::endsWith(lemma, lemmaSuffix)) {
            return lemma.substr(0, lemma.size() - lemmaSuffix.size()) + suffix;
        }
    }

    return lemma + suffix;
}

::std::u16string Inflector_Inflection::lemmatize(const ::std::u16string& noun) const
{
    std::u16string suffix = getSuffix();

    if (inflectionPattern.lemmaSuffixesLen != 0 && ::morphuntion::util::StringViewUtils::endsWith(noun, suffix)) {
        return noun.substr(0, noun.size() - suffix.size())
                + inflectionPattern.suffixes.getString(inflectionPattern.inflectorDictionary.inflectionsArray.read(inflectionPattern.lemmaSuffixesOffset));
    }

    return noun;
}

::std::u16string Inflector_Inflection::getSuffix() const
{
    return inflectionPattern.suffixes.getString(suffixId);
}

int64_t Inflector_Inflection::getGrammemes() const {
    return grammemes;
}

const morphuntion::dictionary::Inflector_InflectionPattern * Inflector_Inflection::getInflectionPattern() const {
    return &inflectionPattern;
}

} // namespace morphuntion::dictionary
