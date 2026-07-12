/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/Inflector.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlAdjectiveInflectionPattern::NlGrammarSynthesizer_NlAdjectiveInflectionPattern(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , declensionFeature(*npc(model.getFeature(NlGrammarSynthesizer::DECLENSION)))
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DUTCH())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::DUTCH()))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryDefinite, {GrammemeConstants::DEFINITENESS_DEFINITE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryIndefinite, {GrammemeConstants::DEFINITENESS_INDEFINITE}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {GrammemeConstants::GENDER_NEUTER}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryComparisonMask, {GrammemeConstants::COMPARISON_POSITIVE, GrammemeConstants::COMPARISON_COMPARATIVE, GrammemeConstants::COMPARISON_SUPERLATIVE}));
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflect(const ::std::u16string& displayString, int64_t wordGrammemes, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto targetDeclension = getDeclension(constraints);
    if (targetDeclension != NlGrammarSynthesizer::Declension::undefined && (wordGrammemes & dictionaryAdjective) != 0) {
        return inflectWithDeclension(displayString, targetDeclension);
    }
    return {};
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflectWithDefiniteness(const ::std::u16string& word, NlGrammarSynthesizer::Gender nounGender, const ::std::u16string& definiteness, NlGrammarSynthesizer::Declension targetDeclension) const
{
    if (!definiteness.empty()) {
        int64_t toConstraintGrammemes;
        if (nounGender == NlGrammarSynthesizer::Gender::neuter && definiteness == GrammemeConstants::DEFINITENESS_INDEFINITE) {
            toConstraintGrammemes = dictionaryIndefinite | dictionaryNeuter;
        }
        else {
            toConstraintGrammemes = dictionaryDefinite;
        }
        ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> patterns;
        inflector.getInflectionPatternsForWord(word, patterns);
        for (const auto& pattern : patterns) {
            if ((pattern.getPartsOfSpeech() & dictionaryAdjective) == 0) {
                continue;
            }
            auto inflection = pattern.reinflect(0, toConstraintGrammemes, word);
            if (!inflection.empty()) {
                return inflection;
            }
        }
    }
    if (targetDeclension == NlGrammarSynthesizer::Declension::undeclined) {
        // The undeclined (base) form isn't reachable by stripping the declension "-e":
        // Dutch undeclension reverses spelling changes (rode->rood, oude->oud, boze->boos),
        // so recover the base form from the dictionary instead of returning the input as-is.
        auto undeclined = undeclineAdjective(word);
        if (!undeclined.empty()) {
            return undeclined;
        }
    }
    return inflectWithDeclension(word, targetDeclension);
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::undeclineAdjective(const ::std::u16string& word) const
{
    // The base (undeclined) form carries only the comparison degree, with no gender/number.
    // Reinflect toward that degree with an empty source grammeme set so the fewest-grammeme
    // (base) form is selected rather than the declined "neuter singular" form.
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, word);
    int64_t degree = wordGrammemes & dictionaryComparisonMask;
    if (degree == 0) {
        return {};
    }
    ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> patterns;
    inflector.getInflectionPatternsForWord(word, patterns);
    for (const auto& pattern : patterns) {
        if ((pattern.getPartsOfSpeech() & dictionaryAdjective) == 0) {
            continue;
        }
        auto base = pattern.reinflect(0, degree, word);
        if (!base.empty()) {
            return base;
        }
    }
    return {};
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflectWithDeclension(const ::std::u16string& lemma, NlGrammarSynthesizer::Declension targetDeclension)
{
    if (targetDeclension == NlGrammarSynthesizer::Declension::undeclined) {
        return lemma;
    }
    else if (targetDeclension == NlGrammarSynthesizer::Declension::declined) {
        return heuristicDeclensionInflection(lemma);
    }

    return {};
}

::std::u16string NlGrammarSynthesizer_NlAdjectiveInflectionPattern::heuristicDeclensionInflection(const ::std::u16string& lemma)
{
    auto length = lemma.length();
    if (length < 2) {
        return lemma;
    }
    if (lemma.ends_with(u"en") || lemma.ends_with(u"e")) {
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
        if (!lemma.ends_with(u"ij") && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM3) && ::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM2) && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(charAtM1)) {
            return lemma + charAtM1 + u'e';
        }
    }
    return lemma + u"e";
}

::std::vector<::std::u16string> NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    NlGrammarSynthesizer::Declension targetDeclension = getDeclension(constraints); // Shouldn't be null, checked by isSupported

    for (auto index : indexesOfSignificant) {
        npc(tokens)->at(index) = inflectWithDeclension((*tokens)[index], targetDeclension);
    }
    return *npc(tokens);
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

NlGrammarSynthesizer::Declension
NlGrammarSynthesizer_NlAdjectiveInflectionPattern::getDeclension(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    return NlGrammarSynthesizer::getDeclension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
}

} // namespace inflection::grammar::synthesis
