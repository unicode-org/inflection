/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>

#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlAdjectiveInflectionPattern::NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

bool NlGrammarSynthesizer_NlAdjectiveInflectionPattern::isSupported(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isAdjective(displayString)) {
        logUnsupportedInflection(displayString, u"not a adjective or not in dictionary");
        return false;
    }
    return isValidDeclensionConstraint(constraints);
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflect(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isSupported(displayString, constraints)) {
        return {};
    }
    // TODO: map to lemma
    const auto& lemma = displayString;
    auto targetDeclension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
    return inflectWithDeclension(lemma, NlGrammarSynthesizer::getDeclension(&targetDeclension));
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflectWithDeclension(const ::std::u16string& lemma, NlGrammarSynthesizer::Declension targetDeclension) const
{
    if (targetDeclension == NlGrammarSynthesizer::Declension::undefined) {
        return {};
    }
    else if (targetDeclension == NlGrammarSynthesizer::Declension::undeclined) {
        return lemma;
    }
    else if (targetDeclension == NlGrammarSynthesizer::Declension::declined) {
        return heuristicDeclensionInflection(lemma);
    }

    return {};
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::heuristicDeclensionInflection(const ::std::u16string& lemma) const
{
    auto length = lemma.length();
    if (length < 2) {
        return lemma;
    }
    if (::inflection::util::StringViewUtils::endsWith(lemma, u"en") || ::inflection::util::StringViewUtils::endsWith(lemma, u"e")) {
        return lemma;
    }
    if (length >= 3) {
        auto charAtM1 = lemma[length - 1];
        auto charAtM2 = lemma[length - 2];
        auto charAtM3 = lemma[length - 3];
        if (isCombinedVowel(charAtM3, charAtM2) &&
               !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM1)) {
            // Replace 's' by 'z' if following a vowel
            if (charAtM1 == u's') {
                charAtM1 = u'z';
            }
            if (charAtM2 == charAtM3) {
                return lemma.substr(0, length - 2) + charAtM1 + u'e';
            } else {
                return lemma.substr(0, length - 1) + charAtM1 + u'e';

            }
        }
        if (!::inflection::util::StringViewUtils::endsWith(lemma, u"ij") && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM3) && ::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM2) && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM1)) {
            return lemma + charAtM1 + u'e';
        }
    }
    return lemma + u"e";
}

bool NlGrammarSynthesizer_NlAdjectiveInflectionPattern::isCombinedVowel(char16_t c1, char16_t c2)
{
    return (::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(c1) && c1 == c2) ||
            (c1 == u'i' && c2 == u'j') ||
            (c1 == u'e' && c2 == u'i') ||
            (c1 == u'i' && c2 == u'e') ||
            (c1 == u'o' && c2 == u'e') ||
            (c1 == u'o' && c2 == u'u') ||
            (c1 == u'a' && c2 == u'u');
}


} // namespace inflection::grammar::synthesis
