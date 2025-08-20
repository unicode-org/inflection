/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupInflector.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/uchar.h>
#include <inflection/npc.hpp>
#include <memory>
#include <algorithm>
#include <map>
#include <optional>
#include <vector>

namespace inflection::grammar::synthesis {
    using dialog::SemanticFeature;
    using dialog::SemanticFeatureModel_DisplayData;
    using dialog::DisplayValue;

    static const icu4cxx::UnicodeSet malayalamInflectableChars(u"[\\u0D00-\\u0D7F]");
    static const icu4cxx::UnicodeSet nonMalayalamChars(u"[\\p{Latin}\\p{Nd}\\p{Punct}]");

    static constexpr auto CASE_NOMINATIVE = u"nominative";
    static constexpr auto CASE_ACCUSATIVE = u"accusative";
    static constexpr auto CASE_DATIVE = u"dative";
    static constexpr auto CASE_GENITIVE = u"genitive";
    static constexpr auto CASE_INSTRUMENTAL = u"instrumental";
    static constexpr auto CASE_LOCATIVE = u"locative";
    static constexpr auto CASE_SOCIATIVE = u"sociative";
    static constexpr auto NUMBER_SINGULAR = u"singular";
    static constexpr auto NUMBER_PLURAL = u"plural";
    static constexpr auto GENDER_MASCULINE = u"masculine";
    static constexpr auto GENDER_FEMININE = u"feminine";
    static constexpr auto GENDER_NEUTER = u"neuter";
    static constexpr auto FORMALITY_FORMAL = u"formal";
    static constexpr auto FORMALITY_INFORMAL = u"informal";
    static constexpr auto CLUSIVITY_INCLUSIVE = u"inclusive";
    static constexpr auto CLUSIVITY_EXCLUSIVE = u"exclusive";
    static constexpr auto PERSON_FIRST = u"first";
    static constexpr auto PERSON_SECOND = u"second";
    static constexpr auto PERSON_THIRD = u"third";
    static constexpr auto TENSE_PAST = u"past";
    static constexpr auto TENSE_PRESENT = u"present";
    static constexpr auto TENSE_FUTURE = u"future";
    static constexpr auto MOOD_INDICATIVE = u"indicative";
    static constexpr auto MOOD_IMPERATIVE = u"imperative";
    static constexpr auto MOOD_SUBJUNCTIVE = u"subjunctive";

    static std::u16string getStrFeature(
        const std::u16string& name,
        const std::map<dialog::SemanticFeature, std::u16string>& features) {
        for (const auto& [key, value] : features) {
            if (key.getName() == name) return value;
        }
        return u"";
    }

    static std::u16string fallbackMalayalamPronoun(
        const std::map<dialog::SemanticFeature, std::u16string>& features) {
        const auto person = getStrFeature(u"person", features);
        const auto number = getStrFeature(u"number", features);
        const auto case_ = getStrFeature(u"case", features);
        (void)person;
        (void)number;
        (void)case_;
        return u"";
    }

    static int32_t buildSuffixKey(const std::vector<std::u16string>& constraintValues) {
        int32_t key = 0;
        for (const auto& val : constraintValues) {
            if (val == CASE_NOMINATIVE) key |= 0x01;
            else if (val == CASE_ACCUSATIVE) key |= 0x02;
            else if (val == CASE_DATIVE) key |= 0x04;
            else if (val == CASE_GENITIVE) key |= 0x08;
            else if (val == NUMBER_SINGULAR) key |= 0x10;
            else if (val == NUMBER_PLURAL) key |= 0x20;
        }
        return key;
    }

    static int32_t buildVerbSuffixKey(const std::vector<std::u16string>& constraintValues) {
        int32_t key = 0;
        for (const auto& val : constraintValues) {
            if (val == PERSON_FIRST) key |= 0x01;
            else if (val == PERSON_SECOND) key |= 0x02;
            else if (val == PERSON_THIRD) key |= 0x04;
            if (val == NUMBER_SINGULAR) key |= 0x10;
            else if (val == NUMBER_PLURAL) key |= 0x20;
            if (val == TENSE_PAST) key |= 0x100;
            else if (val == TENSE_PRESENT) key |= 0x200;
            else if (val == TENSE_FUTURE) key |= 0x400;
            if (val == MOOD_INDICATIVE) key |= 0x1000;
            else if (val == MOOD_IMPERATIVE) key |= 0x2000;
            else if (val == MOOD_SUBJUNCTIVE) key |= 0x4000;
        }
        return key;
    }

    static const std::map<int32_t, std::u16string> malayalamSuffixMap = {
        {0x01 | 0x10, u""},
        {0x01 | 0x20, u"കൾ"},
        {0x04 | 0x10, u"ക്ക്"},
        {0x04 | 0x20, u"കൾക്ക്"},
        {0x08 | 0x10, u"യുടെ"},
        {0x08 | 0x20, u"കളുടെ"},
    };

    static const std::map<int32_t, std::u16string> malayalamVerbSuffixMap = {
        {0x01 | 0x10 | 0x100 | 0x1000, u"ച്ചു"},
        {0x01 | 0x20 | 0x100 | 0x1000, u"ഞ്ഞു"},
        {0x02 | 0x10 | 0x100 | 0x1000, u"ച്ചു"},
        {0x02 | 0x20 | 0x100 | 0x1000, u"ന്നു"},
        {0x04 | 0x10 | 0x100 | 0x1000, u"ച്ചു"},
        {0x04 | 0x20 | 0x100 | 0x1000, u"ന്നു"},
        {0x01 | 0x10 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x01 | 0x20 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x02 | 0x10 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x02 | 0x20 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x04 | 0x10 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x04 | 0x20 | 0x200 | 0x1000, u"ിക്കുന്നു"},
        {0x01 | 0x10 | 0x400 | 0x1000, u" ചെയ്യും"},
        {0x01 | 0x20 | 0x400 | 0x1000, u" ചെയ്യും"},
        {0x02 | 0x10 | 0x400 | 0x1000, u" ചെയ്യും"},
        {0x02 | 0x20 | 0x400 | 0x1000, u" ചെയ്യും"},
        {0x04 | 0x10 | 0x400 | 0x1000, u" ചെയ്യും"},
        {0x04 | 0x20 | 0x400 | 0x1000, u" ചെയ്യും"},
    };

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
        addIfNotEmpty(moodFeature);
        addIfNotEmpty(determinationFeature);
        addIfNotEmpty(pronounTypeFeature);

        const auto posVal = GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature);
        if (!posVal.empty()) vals.push_back(posVal);

        return vals;
    }

    static std::optional<std::u16string> guessFallbackNounInflection(
        const std::u16string& phrase,
        const std::vector<std::u16string>& constraintValues,
        const dialog::DictionaryLookupInflector& dictionaryInflector) {
        std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer(
            inflection::tokenizer::TokenizerFactory::createTokenizer(util::LocaleUtils::MALAYALAM()));
        if (!tokenizer) return std::nullopt;

        std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(
            tokenizer->createTokenChain(phrase));
        if (!tokenChain || tokenChain->getWordCount() == 0) return std::nullopt;

        int lastIndex = -1;
        int64_t wordGrammemes = 0;
        for (int i = 0; i < tokenChain->getWordCount(); ++i) {
            std::u16string token = tokenChain->getTokenValue(i);
            dictionaryInflector.getDictionary().getCombinedBinaryType(&wordGrammemes, token);
            bool isPostposition = false;
            if (isPostposition) break;
            if (inflection::util::UnicodeSetUtils::containsSome(malayalamInflectableChars, token)) {
                lastIndex = i;
            }
        }

        if (lastIndex == -1) return phrase;

        std::vector<std::u16string> tokens;
        for (int i = 0; i < tokenChain->getWordCount(); ++i)
            tokens.push_back(tokenChain->getTokenValue(i));

        std::u16string& head = tokens[lastIndex];
        auto inflected = dictionaryInflector.inflect(head, wordGrammemes, constraintValues);
        if (!inflected.has_value()) {
            int32_t key = buildSuffixKey(constraintValues);
            if ((key & 0x20) != 0) { // plural requested
                if (!head.ends_with(u"കൾ")) head += u"കൾ";
            }
        } else {
            head = *inflected;
        }

        std::u16string result;
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (i > 0) result += u" ";
            result += tokens[i];
        }
        return result;
    }

    static std::optional<std::u16string> guessFallbackVerbInflection(
        const std::u16string& token,
        const std::vector<std::u16string>& constraintValues) {
        int32_t key = buildVerbSuffixKey(constraintValues);
        auto it = malayalamVerbSuffixMap.find(key);
        if (it != malayalamVerbSuffixMap.end()) return token + it->second;
        return std::nullopt;
    }

    std::u16string MlGrammarSynthesizer_MlDisplayFunction::inflectPhrase(
        const std::u16string& phrase,
        const std::vector<std::u16string>& constraintValues,
        bool enableInflectionGuess) const {
        std::unique_ptr<inflection::tokenizer::Tokenizer> tokenizer(
            inflection::tokenizer::TokenizerFactory::createTokenizer(util::LocaleUtils::MALAYALAM()));
        if (!tokenizer) return phrase;

        std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(
            tokenizer->createTokenChain(phrase));
        if (!tokenChain || tokenChain->getWordCount() == 0) return phrase;

        // Copy all tokens into a vector
        std::vector<std::u16string> tokens;
        for (int i = 0; i < tokenChain->getWordCount(); ++i) {
            tokens.push_back(tokenChain->getTokenValue(i));
        }

        // Determine POS from constraintValues
        std::u16string posVal;
        for (const auto& val : constraintValues) {
            if (val == GrammemeConstants::POS_NOUN() ||
                val == GrammemeConstants::POS_PRONOUN() ||
                val == GrammemeConstants::POS_VERB()) {
                posVal = val;
                break;
            }
        }

        // Inflect only the last token if needed
        if (!tokens.empty()) {
            int lastIndex = static_cast<int>(tokens.size()) - 1;
            std::u16string& lastToken = tokens[lastIndex];
            std::optional<std::u16string> inflectedOpt;
            int64_t wordGrammemes = 0;
            dictionaryInflector.getDictionary().getCombinedBinaryType(&wordGrammemes, lastToken);
            inflectedOpt = dictionaryInflector.inflect(lastToken, wordGrammemes, constraintValues);

            if (!inflectedOpt.has_value() && enableInflectionGuess) {
                if (posVal == GrammemeConstants::POS_NOUN() || posVal == GrammemeConstants::POS_PRONOUN()) {
                    // Rebuild full phrase for noun fallback
                    std::u16string fullPhrase;
                    for (size_t i = 0; i < tokens.size(); ++i) {
                        if (i > 0) fullPhrase += u" ";
                        fullPhrase += tokens[i];
                    }
                    inflectedOpt = guessFallbackNounInflection(fullPhrase, constraintValues, dictionaryInflector);
                } else if (posVal == GrammemeConstants::POS_VERB()) {
                    inflectedOpt = guessFallbackVerbInflection(lastToken, constraintValues);
                }
            }

            if (inflectedOpt.has_value()) lastToken = *inflectedOpt;
        }

        // Rebuild the phrase
        std::u16string result;
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (i > 0) result += u" ";
            result += tokens[i];
        }
        return result;
    }

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
                  {CASE_NOMINATIVE, CASE_ACCUSATIVE, CASE_DATIVE, CASE_GENITIVE, CASE_LOCATIVE, CASE_INSTRUMENTAL, CASE_SOCIATIVE},
                  {NUMBER_SINGULAR, NUMBER_PLURAL},
                  {GENDER_MASCULINE, GENDER_FEMININE, GENDER_NEUTER},
                  {FORMALITY_FORMAL, FORMALITY_INFORMAL},
                  {CLUSIVITY_INCLUSIVE, CLUSIVITY_EXCLUSIVE},
                  {PERSON_FIRST, PERSON_SECOND, PERSON_THIRD},
                  {TENSE_PAST, TENSE_PRESENT, TENSE_FUTURE},
                  {MOOD_INDICATIVE, MOOD_IMPERATIVE, MOOD_SUBJUNCTIVE}
              },
              {},
              true) {}

    ::inflection::dialog::DisplayValue* MlGrammarSynthesizer_MlDisplayFunction::getDisplayValue(
    const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData,
    const std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (!displayValue || displayValue->getDisplayString().empty()) {
        if (GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature) == GrammemeConstants::POS_PRONOUN()) {
            std::u16string fallback = fallbackMalayalamPronoun(constraints);
            if (!fallback.empty()) return new DisplayValue(fallback, constraints);
        }
        return nullptr;
    }

    const std::u16string& firstDisplayValue = displayValue->getDisplayString();
    if (constraints.empty() ||
        !inflection::util::UnicodeSetUtils::containsSome(malayalamInflectableChars, firstDisplayValue) ||
        inflection::util::UnicodeSetUtils::containsSome(nonMalayalamChars, firstDisplayValue)) {
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
    if (!inflected.empty() && inflected != firstDisplayValue) {
        return new DisplayValue(inflected, constraints);
    }

    if (GrammarSynthesizerUtil::getFeatureValue(constraints, posFeature) == GrammemeConstants::POS_PRONOUN()) {
        std::u16string fallback = fallbackMalayalamPronoun(constraints);
        if (!fallback.empty()) return new DisplayValue(fallback, constraints);
    }

    return nullptr;
}

    MlGrammarSynthesizer_MlDisplayFunction::~MlGrammarSynthesizer_MlDisplayFunction() = default;

} // namespace inflection::grammar::synthesis