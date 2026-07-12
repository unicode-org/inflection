/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/PlGrammarSynthesizer_PlDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/PlGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <memory>
#include <vector>

// derived from genExemplars when generating suffix_pl.csv
static constexpr int32_t MAX_SUFFIX_LEN = 7;
static constexpr int32_t MIN_STEM_LEN = 2;

namespace inflection::grammar::synthesis {

template <typename T>
static T java_cast(auto* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

PlGrammarSynthesizer_PlDisplayFunction::PlGrammarSynthesizer_PlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(model.getLocale()))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , animacyFeature(*npc(model.getFeature(GrammemeConstants::ANIMACY)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , withPrepositionFeature(*npc(model.getFeature(u"withPreposition")))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::POLISH())))
    , dictionaryInflector(::inflection::util::LocaleUtils::POLISH(),{
        {GrammemeConstants::POS_NUMERAL, GrammemeConstants::POS_NOUN, GrammemeConstants::POS_PRONOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_VERB},
        {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_VOCATIVE},
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER},
        {GrammemeConstants::ANIMACY_HUMAN, GrammemeConstants::ANIMACY_NONHUMAN, GrammemeConstants::ANIMACY_ANIMATE, GrammemeConstants::ANIMACY_INANIMATE},
    })
    , suffixToExemplar(inflection::util::ResourceLocator::getRootForLocale(inflection::util::LocaleUtils::POLISH()) + u"/exemplar/suffix_" + inflection::util::LocaleUtils::POLISH().toString() + u".bist")
    , caseLookupFunction(*npc(java_cast<const inflection::dialog::DictionaryLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(GrammemeConstants::CASE))))))
    , genderLookupFunction(*npc(java_cast<const inflection::dialog::DictionaryLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(GrammemeConstants::GENDER))))))
    , numberLookupFunction(*npc(java_cast<const inflection::dialog::DictionaryLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(GrammemeConstants::NUMBER))))))
    , animacyLookupFunction(*npc(java_cast<const inflection::dialog::DictionaryLookupFunction*>(model.getDefaultFeatureFunction(*npc(model.getFeature(GrammemeConstants::ANIMACY))))))
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdposition, {GrammemeConstants::POS_ADPOSITION}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPronoun, {GrammemeConstants::POS_PRONOUN}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE}));
}

PlGrammarSynthesizer_PlDisplayFunction::~PlGrammarSynthesizer_PlDisplayFunction() = default;

::std::optional<::std::u16string> PlGrammarSynthesizer_PlDisplayFunction::inflectWord(::std::u16string_view word, int64_t& wordType, const ::std::vector<::std::u16string>& constraints, const ::std::vector<::std::u16string>& disambiguationGrammemeValues, bool enableInflectionGuess) const
{
    ::std::optional<::std::u16string> inflectedWord;
    if (wordType != 0) {
        inflectedWord = dictionaryInflector.inflect(word, wordType, constraints, disambiguationGrammemeValues);
        if (inflectedWord) {
            return *inflectedWord;
        }
    }
    for (auto prefix : {NEGATED_PREFIX, MOST_PREFIX}) {
        ::std::u16string lowerCasedWord;
        inflection::util::StringViewUtils::lowercase(&lowerCasedWord, word, ::inflection::util::LocaleUtils::POLISH());

        if (lowerCasedWord.starts_with(prefix)) {
            std::u16string_view unprefixedWord(word);
            unprefixedWord.remove_prefix(prefix.length());
            if (dictionary.getCombinedBinaryType(&wordType, unprefixedWord) != nullptr) {
                inflectedWord = dictionaryInflector.inflect(unprefixedWord, wordType, constraints, disambiguationGrammemeValues);
                if (inflectedWord) {
                    inflectedWord->insert(0, word.substr(0, prefix.length()));
                    return *inflectedWord;
                }
            }
        }
    }
    if (enableInflectionGuess && wordType == 0 && word.length() > MIN_STEM_LEN) {
        for (int32_t suffixLen = std::min(MAX_SUFFIX_LEN, static_cast<int32_t>(word.length() - MIN_STEM_LEN)); suffixLen > 0; --suffixLen) {
            std::u16string_view suffix = word.substr(word.length() - suffixLen);
            auto exemplarResult = suffixToExemplar.findTarget(suffix);
            if (exemplarResult.has_value()) {
                const auto& exemplar = *exemplarResult;
                int64_t exemplarGrammemes = 0;
                dictionary.getCombinedBinaryType(&exemplarGrammemes, exemplar);
                if (exemplarGrammemes != 0) {
                    auto guessedResult = dictionaryInflector.inflectExemplar(word, exemplar, exemplarGrammemes, constraints, disambiguationGrammemeValues);
                    if (guessedResult.has_value()) {
                        return *guessedResult;
                    }
                }
            }
        }
    }
    return {};
}

::std::optional<::std::u16string> PlGrammarSynthesizer_PlDisplayFunction::inflectTokenChain(const ::inflection::tokenizer::TokenChain& tokenChain, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&numberFeature, &genderFeature, &caseFeature}));
    auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));

    const inflection::tokenizer::Token* headTokenPtr = GrammarSynthesizerUtil::getLastSignificantToken(tokenChain.getTail());
    if (headTokenPtr == nullptr) {
        return {};
    }
    const auto& headToken = *npc(headTokenPtr);
    const auto& headWord = headToken.getValue();
    std::u16string inflectedHeadWord(headWord);
    int64_t headWordGrammemes = 0;
    dictionary.getCombinedBinaryType(&headWordGrammemes, headWord);
    auto inflectedToken(inflectWord(headWord, headWordGrammemes, constraintsVec, disambiguationGrammemeValues, enableInflectionGuess));
    if (!inflectedToken) {
        if (headWordGrammemes == 0) {
            return {};
        }
    } else {
        inflectedHeadWord = *inflectedToken;
    }
    auto numberConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    if (numberConstraint.empty()) {
        numberConstraint = numberLookupFunction.determine(headWord);
    }
    auto caseConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    if (caseConstraint.empty()) {
        caseConstraint = caseLookupFunction.determine(headWord);
    }
    auto genderConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
    if (numberConstraint == GrammemeConstants::NUMBER_SINGULAR && genderConstraint.empty()) {
        genderConstraint = genderLookupFunction.determine(headWord);
    }
    auto animacyConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, animacyFeature));
    // Polish plural adjective agreement is virile (masculine personal) vs non-virile, which the
    // dictionary encodes on the human/nonhuman animacy axis. Determine it from the head noun for
    // plural (in addition to the masculine-singular accusative case) so modifiers agree correctly.
    if (animacyConstraint.empty()
        && (genderConstraint == GrammemeConstants::GENDER_MASCULINE
            || numberConstraint == GrammemeConstants::NUMBER_PLURAL)) {
        animacyConstraint = animacyLookupFunction.determine(headWord);
    }
    // The head noun's animacy uses the animate/inanimate axis, but plural adjectives agree on the
    // virile/non-virile (human/nonhuman) axis: only a masculine-personal (human) head is virile.
    // Collapse onto that axis so the adjective forms (tagged human/nonhuman) can be selected.
    if (numberConstraint == GrammemeConstants::NUMBER_PLURAL) {
        animacyConstraint = (animacyConstraint == GrammemeConstants::ANIMACY_HUMAN)
            ? ::std::u16string(GrammemeConstants::ANIMACY_HUMAN)
            : ::std::u16string(GrammemeConstants::ANIMACY_NONHUMAN);
    }

    constraintsVec = {numberConstraint, genderConstraint, caseConstraint, animacyConstraint};
    std::erase_if(constraintsVec,
                    [](std::u16string_view str) {
                        return str.empty();
                    });

    disambiguationGrammemeValues.clear();
    disambiguationGrammemeValues.emplace_back(GrammemeConstants::POS_ADJECTIVE);

    std::u16string result;
    bool inflectingBeforeNoun = true;
    for (const auto& token : tokenChain) {
        if (token.getLength() == 0) {
            continue;
        }
        if (inflectingBeforeNoun && token.getStartChar() == headToken.getStartChar()) {
            result += inflectedHeadWord;
            inflectingBeforeNoun = false;
            continue;
        }
        const auto& tokenVal = token.getValue();
        if (inflectingBeforeNoun && token.isSignificant()) {
            int64_t wordGrammemes = 0;
            dictionary.getCombinedBinaryType(&wordGrammemes, tokenVal);
            if ((wordGrammemes & dictionaryAdposition) != 0) {
                inflectedToken = PlGrammarSynthesizer::getPreposition(&token);
            }
            else {
                inflectedToken = inflectWord(tokenVal, wordGrammemes, constraintsVec, disambiguationGrammemeValues, enableInflectionGuess);
                if (!inflectedToken && !animacyConstraint.empty()) {
                    // Retry without the determined virility: a form may not carry the human/nonhuman
                    // tag (data using a different convention), but still inflects by number/gender/case.
                    std::vector<std::u16string> withoutAnimacy({numberConstraint, genderConstraint, caseConstraint});
                    std::erase_if(withoutAnimacy,
                                    [](std::u16string_view str) {
                                        return str.empty();
                                    });
                    inflectedToken = inflectWord(tokenVal, wordGrammemes, withoutAnimacy, disambiguationGrammemeValues, enableInflectionGuess);
                }
                if (!inflectedToken && (wordGrammemes & dictionaryPronoun) != 0) {
                    inflectedToken = tokenVal;
                }
            }
            if (!inflectedToken) {
                ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
                dictionaryInflector.getInflector().getInflectionPatternsForWord(tokenVal, inflectionPatterns);
                if (wordGrammemes != 0 && inflectionPatterns.empty()) {
                    inflectedToken = tokenVal;
                }
                else if (animacyConstraint == GrammemeConstants::ANIMACY_HUMAN) {
                    std::vector<std::u16string> animateAdjective({numberConstraint, genderConstraint, caseConstraint, GrammemeConstants::ANIMACY_ANIMATE});
                    std::erase_if(animateAdjective,
                                    [](std::u16string_view str) {
                                        return str.empty();
                                    });
                    inflectedToken = inflectWord(tokenVal, wordGrammemes, animateAdjective, disambiguationGrammemeValues, enableInflectionGuess);
                }
                if (!inflectedToken) {
                    return {};
                }
            }
            result += *inflectedToken;
        }
        else {
            result += tokenVal;
        }
    }

    return result;
}

::inflection::dialog::DisplayValue * PlGrammarSynthesizer_PlDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    auto displayValueConstraints(GrammarSynthesizerUtil::mergeConstraintsWithDisplayValue(*displayValue, constraints));

    if (!constraints.empty()) {
        int64_t wordGrammemes = 0;
        std::optional<std::u16string> inflectionResult;
        if (dictionary.getCombinedBinaryType(&wordGrammemes, displayString) != nullptr) {
            auto numberConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
            auto genderConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature));
            auto caseConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
            auto animacyConstraint(GrammarSynthesizerUtil::getFeatureValue(constraints, animacyFeature));
            // Polish plural adjective/pronoun agreement is virile (masculine personal) vs non-virile,
            // encoded on the human/nonhuman animacy axis. Unlike the head noun, the non-virile forms
            // are shared across genders and stored gender-neutrally under nonhuman. When a non-human
            // animacy (animate/inanimate) is requested for a plural agreement word in a case that
            // distinguishes virility (nominative/accusative/vocative), collapse it onto nonhuman and
            // drop the gender so the shared non-virile form is selected instead of the virile one.
            if (numberConstraint == GrammemeConstants::NUMBER_PLURAL
                && (wordGrammemes & (dictionaryAdjective | dictionaryPronoun)) != 0
                && (animacyConstraint == GrammemeConstants::ANIMACY_ANIMATE || animacyConstraint == GrammemeConstants::ANIMACY_INANIMATE)
                && (caseConstraint == GrammemeConstants::CASE_NOMINATIVE
                    || caseConstraint == GrammemeConstants::CASE_ACCUSATIVE
                    || caseConstraint == GrammemeConstants::CASE_VOCATIVE)) {
                animacyConstraint = GrammemeConstants::ANIMACY_NONHUMAN;
                genderConstraint.clear();
            }
            std::vector<std::u16string> constraintsVec({numberConstraint, genderConstraint, caseConstraint, animacyConstraint});
            std::erase_if(constraintsVec, [](std::u16string_view str) { return str.empty(); });
            const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));
            inflectionResult = inflectWord(displayString, wordGrammemes, constraintsVec, disambiguationGrammemeValues, enableInflectionGuess);
        }
        else {
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            inflectionResult = inflectTokenChain(*tokenChain, constraints, enableInflectionGuess);
        }
        if (inflectionResult) {
            displayString = *inflectionResult;
        }

        auto preposition(GrammarSynthesizerUtil::getFeatureValue(constraints, withPrepositionFeature));
        if (!preposition.empty()) {
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
            auto firstSignificantWord(GrammarSynthesizerUtil::getFirstSignificantToken(tokenChain->getHead()));
            if (firstSignificantWord != nullptr && npc(firstSignificantWord)->getCleanValue() != preposition) {
                displayString = PlGrammarSynthesizer::getPreposition(preposition, npc(firstSignificantWord)->getCleanValue())
                    + u" " + displayString;
            }
        }
    }
    return new ::inflection::dialog::DisplayValue(displayString, displayValueConstraints);
}

} // namespace inflection::grammar::synthesis
