/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_EnDisplayFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/tokenizer/Token_Tail.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

EnGrammarSynthesizer_EnDisplayFunction::EnGrammarSynthesizer_EnDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , dictionaryInflector(::morphuntion::util::LocaleUtils::ENGLISH(), {
            {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()}
        }, {}, true
    )
    , dictionary(dictionaryInflector.getDictionary())
    , APOSTROPHES(u"'’ʼ")
    , S_LETTERS(u"sS")
    , inflector(::morphuntion::dictionary::Inflector::getInflector(::morphuntion::util::LocaleUtils::ENGLISH()))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::ENGLISH()))
    , definiteArticleLookupFunction(model, EnGrammarSynthesizer::ARTICLE_DEFINITE, u"the")
    , indefiniteArticleLookupFunction(model, EnGrammarSynthesizer::ARTICLE_INDEFINITE)
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, EnGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, EnGrammarSynthesizer::ARTICLE_INDEFINITE)
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&pluralProperty, {GrammemeConstants::NUMBER_PLURAL()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&singularProperty, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&prepositionProperty, {u"adposition"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&abbreviationProperty, {u"abbreviation"}));
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->caseFeature = model.getFeature(GrammemeConstants::CASE);
    this->speakFeature = model.getFeature(::morphuntion::dialog::SemanticFeatureModel::SPEAK);
    this->partOfSpeechFeature = model.getFeature(GrammemeConstants::POS);
}

EnGrammarSynthesizer_EnDisplayFunction::~EnGrammarSynthesizer_EnDisplayFunction()
{
}

const ::std::map<::std::u16string_view, ::std::u16string_view>& EnGrammarSynthesizer_EnDisplayFunction::POSSESSIVE_DETERMINERS()
{
    static auto POSSESSIVE_DETERMINERS_ = new ::std::map<::std::u16string_view, ::std::u16string_view>({
        {u"i", u"my"},
        {u"me", u"my"},
        {u"he", u"his"},
        {u"him", u"his"},
        {u"she", u"her"},
        {u"it", u"its"},
        {u"we", u"our"},
        {u"us", u"our"},
        {u"you", u"your"},
        {u"they", u"their"},
        {u"them", u"their"},
    });
    return *npc(POSSESSIVE_DETERMINERS_);
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue* EnGrammarSynthesizer_EnDisplayFunction::getDisplayValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string displayString = displayValue->getDisplayString();
    if (displayString.empty()) {
        return nullptr;
    }
    auto displayValueConstraints(constraints);
    ::std::u16string countString = GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature);
    if (countString == GrammemeConstants::NUMBER_PLURAL() || countString == GrammemeConstants::NUMBER_SINGULAR()) {
        auto result = inflectPhrase(displayString, constraints, enableInflectionGuess);
        if (!result && !enableInflectionGuess) {
            return nullptr;
        }
        displayString = *result;
    } else {
        // We only add this speak value if it's not morphologically changing.
        auto speakValue = morphuntion::dialog::DefaultArticleLookupFunction::getFeatureValue(displayValue->getConstraintMap(), *npc(speakFeature));
        if (speakValue != nullptr) {
            displayValueConstraints.emplace(*npc(speakFeature), *npc(speakValue));
        }
    }

    ::std::u16string caseString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
    if (caseString == GrammemeConstants::CASE_GENITIVE()) {
        displayString = inflectPossessive(displayString, displayValueConstraints);
    }

    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, displayValueConstraints), constraints);
}

::std::optional<::std::u16string> EnGrammarSynthesizer_EnDisplayFunction::inflectPhrase(const std::u16string &originalString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    int64_t significantTokenProperties = 0;
    bool isKnown = dictionary.getCombinedBinaryType(&significantTokenProperties, originalString) != nullptr;
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {countFeature}));
    const auto dismbiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {partOfSpeechFeature}));

    const auto inflectResult = dictionaryInflector.inflect(originalString, constraintsVec, dismbiguationGrammemeValues);

    if (inflectResult) {
        // It's a known word or phrase
        return *inflectResult;
    }
    else if (isKnown) {
        // It's a known word or phrase, but there isn't a way to inflect it the desired way.
        if (enableInflectionGuess) {
            return originalString;
        }
        return {};
    }

    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(originalString)));
    // Modify the last word before the first preposition, or the last word
    const morphuntion::tokenizer::Token* significantToken = nullptr;
    for (const auto& token : *tokenChain) {
        if (!token.isSignificant()) {
            continue;
        }
        int64_t combinedType = 0;
        dictionary.getCombinedBinaryType(&combinedType, token.getValue());
        if (significantToken != nullptr && ((combinedType & prepositionProperty) == prepositionProperty)) {
            break;
        }
        significantToken = &token;
        significantTokenProperties = combinedType;
    }
    if (significantToken == nullptr && enableInflectionGuess) {
        return originalString;
    }
    if (significantToken != nullptr && (enableInflectionGuess || significantTokenProperties != 0)) {
        const auto significantWord(npc(significantToken)->getValue());
        auto inflectionResult(dictionaryInflector.inflect(significantWord, constraintsVec, dismbiguationGrammemeValues));
        if (!inflectionResult) {
            if (!enableInflectionGuess) {
                return {};
            }
            if (GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature) == GrammemeConstants::NUMBER_PLURAL()) {
                inflectionResult = guessPluralInflection(significantWord);
            } else {
                inflectionResult = guessSingularInflection(significantWord);
            }
        }
        return originalString.substr(0, npc(significantToken)->getStartChar()) + *inflectionResult + originalString.substr(npc(significantToken)->getEndChar());
    }
    return {};
}

::std::u16string EnGrammarSynthesizer_EnDisplayFunction::guessPluralInflection(const ::std::u16string& displayString) const {
    if (::morphuntion::util::StringViewUtils::endsWith(displayString, u"s")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"ch")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"sh")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"x")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"z"))
    {
        return displayString + u"es";
    }
    else if (displayString.length() > 2 && ::morphuntion::util::StringViewUtils::endsWith(displayString, u"y") && !::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(displayString[displayString.length() - 2])) {
        return displayString.substr(0, displayString.length() - 1) + u"ies";
    }
    return displayString + u"s";
}

::std::u16string EnGrammarSynthesizer_EnDisplayFunction::guessSingularInflection(const std::u16string &displayString) const {
    if (::morphuntion::util::StringViewUtils::endsWith(displayString, u"ses")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"ches")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"shes")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"xes")
        || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"zes"))
    {
        return displayString.substr(0, displayString.length() - 2);
    }
    if (::morphuntion::util::StringViewUtils::endsWith(displayString, u"ies"))
    {
        return displayString.substr(0, displayString.length() - 3) + u"y";
    }
    if (::morphuntion::util::StringViewUtils::endsWith(displayString, u"s"))
    {
        return displayString.substr(0, displayString.length() - 1);
    }
    return displayString;
}

::std::u16string EnGrammarSynthesizer_EnDisplayFunction::inflectPossessive(const ::std::u16string& displayString, ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& valueConstraints) const
{
    ::std::u16string lowercase;
    ::morphuntion::util::StringViewUtils::lowercase(&lowercase, displayString, ::morphuntion::util::LocaleUtils::ENGLISH());
    auto possessiveException = POSSESSIVE_DETERMINERS().find(lowercase);
    if (possessiveException != POSSESSIVE_DETERMINERS().end()) {
        return ::std::u16string(possessiveException->second);
    }
    else if (!displayString.empty()) {
        ::std::u16string suffixStr(u"’s");
        if (displayString.length() > 2) {
            auto suffix(displayString.substr(displayString.length() - 2, 2));
            if (suffix.find_first_of(APOSTROPHES) != std::u16string::npos && suffix.find_first_of(S_LETTERS) != std::u16string::npos) {
                suffixStr.clear();
            }
            else {
                if (::morphuntion::util::StringViewUtils::endsWith(suffix, u"s")) {
                    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
                    if (dictionary.hasAllProperties(npc(npc(tokenChain->getTail())->getPrevious())->getValue(), pluralProperty)) {
                        suffixStr = u"’";
                    }
                }
            }
        }
        auto speakValue = morphuntion::dialog::DefaultArticleLookupFunction::getFeatureValue(valueConstraints, *npc(speakFeature));
        if (speakValue != nullptr) {
            valueConstraints[*npc(speakFeature)] = *npc(speakValue) + suffixStr;
        }
        return displayString + suffixStr;
    }
    return displayString;
}

} // namespace morphuntion::grammar::synthesis
