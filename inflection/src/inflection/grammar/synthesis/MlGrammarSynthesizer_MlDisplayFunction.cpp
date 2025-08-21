/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <map>

namespace inflection::grammar::synthesis {

using dialog::SemanticFeature;
using dialog::SemanticFeatureModel_DisplayData;
using dialog::DisplayValue;

static const icu4cxx::UnicodeSet& malayalamInflectableChars =
    ::inflection::lang::StringFilterUtil::MALAYALAM_SCRIPT();
static const icu4cxx::UnicodeSet nonMalayalamChars(u"[\\p{Latin}\\p{Nd}\\p{Punct}]");

static constexpr auto FORMALITY_FORMAL = u"formal";
static constexpr auto FORMALITY_INFORMAL = u"informal";
static constexpr auto CLUSIVITY_INCLUSIVE = u"inclusive";
static constexpr auto CLUSIVITY_EXCLUSIVE = u"exclusive";
static constexpr auto MOOD_SUBJUNCTIVE = u"subjunctive";

MlGrammarSynthesizer_MlDisplayFunction::MlGrammarSynthesizer_MlDisplayFunction(
    const ::inflection::dialog::SemanticFeatureModel& model)
    : caseFeature(*npc(model.getFeature(GrammemeConstants::CASE))),
      numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER))),
      genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER))),
      posFeature(*npc(model.getFeature(GrammemeConstants::POS))),
      formalityFeature(*npc(model.getFeature(u"formality"))),
      clusivityFeature(*npc(model.getFeature(u"clusivity"))),
      personFeature(*npc(model.getFeature(GrammemeConstants::PERSON))),
      tenseFeature(*npc(model.getFeature(u"tense"))),
      moodFeature(*npc(model.getFeature(u"mood"))),
      pronounTypeFeature(*npc(model.getFeature(u"pronounType"))),
      determinationFeature(*npc(model.getFeature(u"determination"))),
      dictionaryInflector(
          util::LocaleUtils::MALAYALAM(),
          {
              {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_VERB(), GrammemeConstants::POS_PRONOUN()},
              {GrammemeConstants::CASE_NOMINATIVE(), GrammemeConstants::CASE_ACCUSATIVE(), GrammemeConstants::CASE_DATIVE(),
               GrammemeConstants::CASE_GENITIVE(), GrammemeConstants::CASE_LOCATIVE(), GrammemeConstants::CASE_INSTRUMENTAL(),
               GrammemeConstants::CASE_SOCIATIVE()},
              {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
              {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()},
              {FORMALITY_FORMAL, FORMALITY_INFORMAL},
              {CLUSIVITY_INCLUSIVE, CLUSIVITY_EXCLUSIVE},
              {GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND(), GrammemeConstants::PERSON_THIRD()},
              {GrammemeConstants::TENSE_PAST(), GrammemeConstants::TENSE_PRESENT(), GrammemeConstants::TENSE_FUTURE()},
              {GrammemeConstants::MOOD_INDICATIVE(), GrammemeConstants::MOOD_IMPERATIVE(), MOOD_SUBJUNCTIVE} // local constant
          },
          {},
          true) {}

static std::vector<std::u16string> buildConstraintVector(
    const std::map<SemanticFeature, std::u16string>& constraints,
    const SemanticFeature& posFeature,
    const SemanticFeature& caseFeature,
    const SemanticFeature& numberFeature,
    const SemanticFeature& genderFeature,
    const SemanticFeature& formalityFeature,
    const SemanticFeature& clusivityFeature,
    const SemanticFeature& personFeature,
    const SemanticFeature& determinationFeature,
    const SemanticFeature& tenseFeature,
    const SemanticFeature& moodFeature,
    const SemanticFeature& pronounTypeFeature) {
    
    std::vector<std::u16string> vals;
    auto addIfNotEmpty = [&](const SemanticFeature& f) {
        const auto v = GrammarSynthesizerUtil::getFeatureValue(constraints, f);
        if (!v.empty()) vals.push_back(v);
    };

    addIfNotEmpty(caseFeature);
    addIfNotEmpty(numberFeature);
    addIfNotEmpty(genderFeature);
    addIfNotEmpty(formalityFeature);
    addIfNotEmpty(clusivityFeature);
    addIfNotEmpty(personFeature);
    addIfNotEmpty(tenseFeature);
    addIfNotEmpty(moodFeature);  // subjunctive now correctly included
    addIfNotEmpty(determinationFeature);
    addIfNotEmpty(pronounTypeFeature);

    const auto posVal = GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature);
    if (!posVal.empty()) vals.push_back(posVal);

    return vals;
}

// Fallback noun/verb inflection unchanged; subjunctive handled via constraintValues
static std::optional<std::u16string> guessFallbackNounInflection(
    const std::u16string& phrase,
    const std::vector<std::u16string>& constraintValues,
    const dialog::DictionaryLookupInflector& dictionaryInflector) 
{
    std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer(
        inflection::tokenizer::TokenizerFactory::createTokenizer(util::LocaleUtils::MALAYALAM()));
    if (!tokenizer) return std::nullopt;

    std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(
        tokenizer->createTokenChain(phrase));
    if (!tokenChain || tokenChain->getWordCount() == 0) return std::nullopt;

    const inflection::tokenizer::Token* lastSignificantToken = nullptr;
    int64_t lastTokenGrammemes = 0;

    for (auto& token : *tokenChain) {
        if (!token.isSignificant()) continue;

        int64_t combinedType = 0;
        dictionaryInflector.getDictionary().getCombinedBinaryType(&combinedType, token.getValue());

        if (inflection::util::UnicodeSetUtils::containsSome(malayalamInflectableChars, token.getValue())) {
            lastSignificantToken = &token;
            lastTokenGrammemes = combinedType;
        }
    }

    if (!lastSignificantToken) return phrase;

    std::u16string result;
    for (auto& token : *tokenChain) {
        std::u16string tokenVal = token.getValue();

        if (token.isSignificant() && &token == lastSignificantToken) {
            auto inflected = dictionaryInflector.inflect(tokenVal, lastTokenGrammemes, constraintValues);
            if (!inflected.has_value()) {
                MlGrammarSynthesizer::Number num = MlGrammarSynthesizer::Number::undefined;
                MlGrammarSynthesizer::Case kase = MlGrammarSynthesizer::Case::undefined;
                for (const auto& val : constraintValues) {
                    if (num == MlGrammarSynthesizer::Number::undefined)
                        num = MlGrammarSynthesizer::numberFromConstraint(val);
                    if (kase == MlGrammarSynthesizer::Case::undefined)
                        kase = MlGrammarSynthesizer::caseFromConstraint(val);
                }

                auto key = MlGrammarSynthesizer::makeLookupKey(num, kase);
                if ((key & 0x20) != 0 && !tokenVal.ends_with(u"കൾ")) {
                    tokenVal += MlGrammarSynthesizer::getSuffix(key);
                }
            } else {
                tokenVal = *inflected;
            }
        }

        if (!result.empty()) result += u" ";
        result += tokenVal;
    }

    return result;
}

static std::optional<std::u16string> guessFallbackVerbInflection(
    const std::u16string& token,
    const std::vector<std::u16string>& constraintValues) 
{
    auto key = MlGrammarSynthesizer::buildVerbSuffixKey(constraintValues);
    return token + MlGrammarSynthesizer::getVerbSuffix(key);
}

std::u16string MlGrammarSynthesizer_MlDisplayFunction::inflectPhrase(
    const std::u16string& phrase,
    const std::vector<std::u16string>& constraintValues,
    bool enableInflectionGuess) const 
{
    std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer(
        inflection::tokenizer::TokenizerFactory::createTokenizer(util::LocaleUtils::MALAYALAM()));
    if (!tokenizer) return phrase;

    std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(
        tokenizer->createTokenChain(phrase));
    if (!tokenChain || tokenChain->getWordCount() == 0) return phrase;

    std::u16string posVal;
    for (const auto& val : constraintValues) {
        if (val == GrammemeConstants::POS_NOUN() ||
            val == GrammemeConstants::POS_PRONOUN() ||
            val == GrammemeConstants::POS_VERB()) 
        {
            posVal = val;
            break;
        }
    }

    const inflection::tokenizer::Token* lastSignificantToken = nullptr;
    for (const auto& token : *tokenChain) {
        if (token.isSignificant()) lastSignificantToken = &token;
    }

    std::u16string result;
    for (const auto& token : *tokenChain) {
        if (!result.empty() && token.isSignificant()) result += u" ";
        std::u16string tokenVal = token.getValue();

        if (&token == lastSignificantToken) {
            int64_t lastTokenGrammemes = 0;
            dictionaryInflector.getDictionary().getCombinedBinaryType(&lastTokenGrammemes, tokenVal);

            auto inflectedOpt = dictionaryInflector.inflect(tokenVal, lastTokenGrammemes, constraintValues);

            if (!inflectedOpt.has_value() && enableInflectionGuess) {
                if (posVal == GrammemeConstants::POS_NOUN() || posVal == GrammemeConstants::POS_PRONOUN()) {
                    std::u16string fullPhrase;
                    for (const auto& t : *tokenChain) fullPhrase += t.getValue();
                    inflectedOpt = guessFallbackNounInflection(fullPhrase, constraintValues, dictionaryInflector);
                } else if (posVal == GrammemeConstants::POS_VERB()) {
                    inflectedOpt = guessFallbackVerbInflection(tokenVal, constraintValues);
                }
            }

            if (inflectedOpt.has_value()) {
                tokenVal = *inflectedOpt;
            } else {
                MlGrammarSynthesizer::Number num = MlGrammarSynthesizer::Number::undefined;
                MlGrammarSynthesizer::Case kase = MlGrammarSynthesizer::Case::undefined;
                for (const auto& val : constraintValues) {
                    if (num == MlGrammarSynthesizer::Number::undefined)
                        num = MlGrammarSynthesizer::numberFromConstraint(val);
                    if (kase == MlGrammarSynthesizer::Case::undefined)
                        kase = MlGrammarSynthesizer::caseFromConstraint(val);
                }

                auto key = MlGrammarSynthesizer::makeLookupKey(num, kase);
                if ((key & 0x20) != 0 && !tokenVal.ends_with(u"കൾ")) {
                    tokenVal += MlGrammarSynthesizer::getSuffix(key);
                }
            }
        }

        result += tokenVal;
    }

    return result;
}

::inflection::dialog::DisplayValue* MlGrammarSynthesizer_MlDisplayFunction::getDisplayValue(
    const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData,
    const std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (!displayValue || displayValue->getDisplayString().empty()) return nullptr;

    const std::u16string& firstDisplayValue = displayValue->getDisplayString();
    if (constraints.empty() ||
        !inflection::util::UnicodeSetUtils::containsSome(malayalamInflectableChars, firstDisplayValue) ||
        inflection::util::UnicodeSetUtils::containsSome(nonMalayalamChars, firstDisplayValue)) 
    {
        return new DisplayValue(firstDisplayValue, constraints);
    }

    std::vector<std::u16string> constraintValues = buildConstraintVector(
        constraints,
        posFeature,
        caseFeature,
        numberFeature,
        genderFeature,
        formalityFeature,
        clusivityFeature,
        personFeature,
        determinationFeature,
        tenseFeature,
        moodFeature,
        pronounTypeFeature);

    std::u16string inflected = inflectPhrase(firstDisplayValue, constraintValues, enableInflectionGuess);
    if (!inflected.empty() && inflected != firstDisplayValue) return new DisplayValue(inflected, constraints);

    return nullptr;
}

MlGrammarSynthesizer_MlDisplayFunction::~MlGrammarSynthesizer_MlDisplayFunction() = default;

} // namespace inflection::grammar::synthesis
