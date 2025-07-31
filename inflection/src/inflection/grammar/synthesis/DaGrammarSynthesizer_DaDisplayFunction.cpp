/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DaGrammarSynthesizer_DaDisplayFunction.hpp>

#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <algorithm>
#include <memory>
#include <unicode/uchar.h>

namespace inflection::grammar::synthesis {

DaGrammarSynthesizer_DaDisplayFunction::DaGrammarSynthesizer_DaDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , posFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DANISH())))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::DANISH()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::DANISH()))
    , genderLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()}),
    dictionaryInflector(::inflection::util::LocaleUtils::DANISH(), {
        {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
        {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
        {GrammemeConstants::CASE_NOMINATIVE(), GrammemeConstants::CASE_GENITIVE()}
        }, {}, true)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
}

DaGrammarSynthesizer_DaDisplayFunction::~DaGrammarSynthesizer_DaDisplayFunction()
{
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectString(const ::std::u16string& lemma, int64_t wordGrammemes, const ::std::u16string& targetCount, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetCase, const ::std::u16string& targetGender, bool targetIsANoun) const
{
    ::std::u16string inflection;
    ::std::u16string count(targetCount);
    ::std::u16string definiteness(targetDefiniteness);

    if (count.empty()) {
        count = GrammemeConstants::NUMBER_SINGULAR();
    }
    if (definiteness.empty()) {
        definiteness = GrammemeConstants::DEFINITENESS_INDEFINITE();
    }
    if (!targetIsANoun) {
        return inflectAdjective(lemma, definiteness, targetGender, count);
    }

    ::std::vector<::std::u16string> constraints;
    constraints.emplace_back(targetCount);
    if (!targetDefiniteness.empty()) {
        constraints.emplace_back(targetDefiniteness);
    }
    if (!targetCase.empty() && targetCase != GrammemeConstants::CASE_NOMINATIVE()) {
        constraints.emplace_back(targetCase);
    }
    if (!targetGender.empty()) {
        constraints.emplace_back(targetGender);
    }

    constexpr std::vector<std::u16string> disambiguationGrammemeValues;
    auto inflectionResult = dictionaryInflector.inflect(lemma, wordGrammemes, constraints, disambiguationGrammemeValues);
    if (inflectionResult) {
        inflection = *inflectionResult;
        // when inflections is made genitive, the `'s` should be lowercased even if the rest
        // of the word was uppercase. See unit test: `NATO [case="genitive"] -> NATO's`
        if (inflection.ends_with(u"'S")) {
            inflection.replace(inflection.length()-2, 2, u"'s");
        }
    }

    return inflection;
}

::inflection::dialog::DisplayValue * DaGrammarSynthesizer_DaDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (!constraints.empty()) {
        int64_t wordGrammemes = 0;
        if (displayString.ends_with(u".")) {
            // These abbreviations don't change definiteness nor change grammatical number.
            displayString = makeGenitiveWhenRequested(constraints, displayString);
        }
        else if (dictionary.getCombinedBinaryType(&wordGrammemes, displayString) != nullptr) {
            displayString = inflectWord(constraints, displayString, wordGrammemes);
        }
        else {
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            displayString = inflectTokenChain(constraints, *npc(tokenChain.get()));
        }
    }
    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectTokenChain(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::inflection::tokenizer::TokenChain& tokenChain) const
{
    return GrammarSynthesizerUtil::inflectSignificantWords(constraints, tokenChain, *this);
}

::std::vector<::std::u16string> DaGrammarSynthesizer_DaDisplayFunction::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto indexOfLastSignificant = indexesOfSignificant.back();
    const auto& wordLast = npc(tokens)->at(indexOfLastSignificant);
    int64_t wordTypeLast = 0;
    if (dictionary.getCombinedBinaryType(&wordTypeLast, wordLast) == nullptr) {
        // Unknown word
        auto lastSignificantToken(npc(tokens)->at(indexOfLastSignificant));
        auto result(makeGenitiveWhenRequested(constraints, lastSignificantToken));
        if (result != lastSignificantToken) {
            npc(tokens)->at(indexOfLastSignificant) = result;
            return *npc(tokens);
        }
        return {};
    }
    const auto& wordFirst = npc(tokens)->at(indexesOfSignificant.front());
    int64_t wordTypeFirst = 0;
    if (indexesOfSignificant.size() == 2 && (dictionary.getCombinedBinaryType(&wordTypeFirst, wordFirst) == nullptr
        || ((wordTypeFirst & dictionaryAdjective) != 0 && (wordTypeFirst & dictionaryNoun) == 0 && (wordTypeLast & dictionaryNoun) != 0)))
    {
        npc(tokens)->at(indexesOfSignificant[0]) = inflectWord(constraints, wordFirst, wordTypeFirst, wordLast, wordTypeLast, false, false);
        npc(tokens)->at(indexesOfSignificant[1]) = inflectWord(constraints, wordLast, wordTypeLast, wordLast, wordTypeLast, true, true);
    } else {
        npc(tokens)->at(indexOfLastSignificant) = inflectWord(constraints, wordLast, wordTypeLast);
    }
    return *npc(tokens);
}

::std::u16string
DaGrammarSynthesizer_DaDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
                                                    const ::std::u16string& displayString,
                                                    int64_t wordGrammemes) const
{
    return inflectWord(constraints, displayString, wordGrammemes, displayString, wordGrammemes, true, false);
}

::std::u16string
DaGrammarSynthesizer_DaDisplayFunction::inflectWord(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
                                                    const ::std::u16string& attributeDisplayString,
                                                    int64_t attributeGrammemes,
                                                    const ::std::u16string& headDisplayString,
                                                    int64_t headGrammemes,
                                                    bool isSuspectedToBeANoun,
                                                    bool isHeadWordWithAttribute) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    ::std::u16string definitenessString;
    if (isHeadWordWithAttribute) {
        definitenessString = GrammemeConstants::DEFINITENESS_INDEFINITE();
    } else {
        definitenessString = GrammarSynthesizerUtil::getFeatureValue(constraints, definitenessFeature);
    }
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);
    if (genderString.empty()) {
        genderString = genderLookupFunction.determine(headDisplayString);
    }
    ::std::u16string posString(GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature));
    auto isAdjective = GrammemeConstants::POS_ADJECTIVE() == posString || (((headGrammemes & dictionaryAdjective) != 0) && ((headGrammemes & dictionaryNoun) == 0));
    auto inflectedString = inflectString(attributeDisplayString, attributeGrammemes, countString, definitenessString, caseString, genderString, !isAdjective && isSuspectedToBeANoun);
    if (isSuspectedToBeANoun && inflectedString.empty()) {
        return makeGenitiveWhenRequested(constraints, attributeDisplayString);
    }
    if (inflectedString.empty()) {
        return attributeDisplayString;
    }
    std::ranges::replace(inflectedString, u'\'', u'’');
    return inflectedString;
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::makeGenitiveWhenRequested(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& displayString) const
{
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    if (GrammemeConstants::CASE_GENITIVE() == caseString && displayString.length() > 1) {
        // See https://sproget.dk/raad-og-regler/typiske-problemer/genitiv-ejefald/genitiv/ for more information
        auto suffix = u_tolower(displayString.back());
        if (suffix == u's' || suffix == u'z' || suffix == u'x') {
            return displayString + u'’';
        }
        if ((inflection::util::StringViewUtils::isAllUpperCase(displayString) && !(bool)u_ispunct(suffix))
            || (bool)u_isdigit(suffix)
            || displayString.ends_with(u".dk"))
        {
            return displayString + u"’s";
        }
        else {
            return displayString + u's';
        }
    }
    return displayString;
}

::std::u16string DaGrammarSynthesizer_DaDisplayFunction::inflectAdjective(const ::std::u16string& lemma, const ::std::u16string& targetDefiniteness, const ::std::u16string& targetGender, const ::std::u16string& targetCount)
{
    auto length = lemma.length();
    if (length < 1) {
        return lemma;
    }
    auto charAtM1 = lemma[length - 1];
    auto endsInIAA = charAtM1 == u'i' || charAtM1 == u'\u00e5';
    auto endsInConsonant = !(endsInIAA || charAtM1 == u'e' || charAtM1 == u'a' || charAtM1 == u'u' || charAtM1 == u'o' || charAtM1 == u'y' || charAtM1 == u'\u00f8' || charAtM1 == u'\u00e6');
    if (GrammemeConstants::NUMBER_PLURAL() == targetCount || GrammemeConstants::DEFINITENESS_DEFINITE() == targetDefiniteness) {
        if (length > 4) {
            auto charAtM3 = lemma[length - 3];
            if (lemma[length - 2] == u'e' && (charAtM1 == u'n' || charAtM1 == u'l' || charAtM1 == u'r') && charAtM3 == lemma[length - 4] && !(charAtM3 == u'i' || charAtM3 == u'\u00e5' || charAtM3 == u'e' || charAtM3 == u'a' || charAtM3 == u'u' || charAtM3 == u'o' || charAtM3 == u'y' || charAtM3 == u'\u00f8' || charAtM3 == u'\u00e6')) {
                return lemma.substr(0, length - 3) + charAtM1 + u'e';
            }
        }
        if (endsInConsonant) {
            return lemma + u"e";
        }
    }
    if (GrammemeConstants::NUMBER_SINGULAR() == targetCount
        && GrammemeConstants::GENDER_NEUTER() == targetGender && (endsInIAA || endsInConsonant) && charAtM1 != u't')
    {
        return lemma + u"t";
    }
    return lemma;
}



} // namespace inflection::grammar::synthesis
