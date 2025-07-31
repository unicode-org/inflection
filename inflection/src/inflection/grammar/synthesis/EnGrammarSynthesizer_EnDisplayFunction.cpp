/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_EnDisplayFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

EnGrammarSynthesizer_EnDisplayFunction::EnGrammarSynthesizer_EnDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , speakFeature(*npc(model.getFeature(::inflection::dialog::SemanticFeatureModel::SPEAK)))
    , partOfSpeechFeature(*npc(model.getFeature(GrammemeConstants::POS)))
    , dictionaryInflector(::inflection::util::LocaleUtils::ENGLISH(), {
            {GrammemeConstants::POS_NOUN(),  GrammemeConstants::POS_ADJECTIVE(), GrammemeConstants::POS_VERB()},
            {GrammemeConstants::PERSON_THIRD(), GrammemeConstants::PERSON_FIRST(), GrammemeConstants::PERSON_SECOND()},
            {GrammemeConstants::NUMBER_SINGULAR(),  GrammemeConstants::NUMBER_PLURAL()}
        }, {}, true
    )
    , dictionary(dictionaryInflector.getDictionary())
    , APOSTROPHES(u"'’ʼ")
    , S_LETTERS(u"sS")
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::ENGLISH()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ENGLISH()))
    , definiteArticleLookupFunction(model, EnGrammarSynthesizer::ARTICLE_DEFINITE, u"the")
    , indefiniteArticleLookupFunction(model, EnGrammarSynthesizer::ARTICLE_INDEFINITE)
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, EnGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, EnGrammarSynthesizer::ARTICLE_INDEFINITE)
    , possessiveDeterminers({
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
    })
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&pluralProperty, {GrammemeConstants::NUMBER_PLURAL()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&singularProperty, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&prepositionProperty, {u"adposition"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&abbreviationProperty, {u"abbreviation"}));
}

EnGrammarSynthesizer_EnDisplayFunction::~EnGrammarSynthesizer_EnDisplayFunction()
{
}

::inflection::dialog::DisplayValue* EnGrammarSynthesizer_EnDisplayFunction::getDisplayValue(const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, bool enableInflectionGuess) const
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
    ::std::u16string countString = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature);
    bool isRequestingPlural = countString == GrammemeConstants::NUMBER_PLURAL();
    if (isRequestingPlural || countString == GrammemeConstants::NUMBER_SINGULAR()) {
        auto result = inflectPhrase(displayString, constraints, enableInflectionGuess);
        if (!result && !enableInflectionGuess) {
            return nullptr;
        }
        displayString = *result;
    } else {
        // We only add this speak value if it's not morphologically changing.
        auto speakValue = inflection::dialog::DefaultArticleLookupFunction::getFeatureValue(displayValue->getConstraintMap(), speakFeature);
        if (speakValue != nullptr) {
            displayValueConstraints.emplace(speakFeature, *npc(speakValue));
        }
    }

    ::std::u16string caseString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
    if (caseString == GrammemeConstants::CASE_GENITIVE()) {
        displayString = inflectPossessive(displayString, displayValueConstraints, isRequestingPlural);
    }

    return definitenessDisplayFunction.addDefiniteness(new ::inflection::dialog::DisplayValue(displayString, displayValueConstraints), constraints);
}

::std::optional<::std::u16string> EnGrammarSynthesizer_EnDisplayFunction::inflectPhrase(const std::u16string &originalString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto constraintsVec(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&numberFeature}));
    const auto disambiguationGrammemeValues(GrammarSynthesizerUtil::convertToStringConstraints(constraints, {&partOfSpeechFeature}));

    int64_t significantTokenProperties = 0;
    if (dictionary.getCombinedBinaryType(&significantTokenProperties, originalString) != nullptr) {
        const auto inflectResult = dictionaryInflector.inflect(originalString, significantTokenProperties, constraintsVec, disambiguationGrammemeValues);
        if (inflectResult) {
            // It's a known word or phrase
            return *inflectResult;
        }
        // It's a known word or phrase, but there isn't a way to inflect it the desired way.
        if (enableInflectionGuess) {
            return originalString;
        }
        return {};
    }

    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(originalString)));
    // Modify the last word before the first preposition, or the last word
    const inflection::tokenizer::Token* significantToken = nullptr;
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
        auto inflectionResult(dictionaryInflector.inflect(significantWord, significantTokenProperties, constraintsVec, disambiguationGrammemeValues));
        if (!inflectionResult) {
            if (!enableInflectionGuess) {
                return {};
            }
            if (GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature) == GrammemeConstants::NUMBER_PLURAL()) {
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
    if (displayString.ends_with(u"s")
        || displayString.ends_with(u"ch")
        || displayString.ends_with(u"sh")
        || displayString.ends_with(u"x")
        || displayString.ends_with(u"z"))
    {
        return displayString + u"es";
    }
    else if (displayString.length() > 2 && displayString.ends_with(u"y") && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(displayString[displayString.length() - 2])) {
        return displayString.substr(0, displayString.length() - 1) + u"ies";
    }
    return displayString + u"s";
}

::std::u16string EnGrammarSynthesizer_EnDisplayFunction::guessSingularInflection(const std::u16string &displayString) const {
    if (displayString.ends_with(u"ses")
        || displayString.ends_with(u"ches")
        || displayString.ends_with(u"shes")
        || displayString.ends_with(u"xes")
        || displayString.ends_with(u"zes"))
    {
        return displayString.substr(0, displayString.length() - 2);
    }
    if (displayString.ends_with(u"ies"))
    {
        return displayString.substr(0, displayString.length() - 3) + u"y";
    }
    if (displayString.ends_with(u"s"))
    {
        return displayString.substr(0, displayString.length() - 1);
    }
    return displayString;
}

::std::u16string EnGrammarSynthesizer_EnDisplayFunction::inflectPossessive(const ::std::u16string& displayString, ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& valueConstraints, bool isRequestingPlural) const
{
    ::std::u16string lowercase;
    ::inflection::util::StringViewUtils::lowercase(&lowercase, displayString, ::inflection::util::LocaleUtils::ENGLISH());
    auto possessiveException = possessiveDeterminers.find(lowercase);
    if (possessiveException != possessiveDeterminers.end()) {
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
                if (suffix.ends_with(u"s")) {
                    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
                    if (isRequestingPlural || dictionary.hasAllProperties(npc(npc(tokenChain->getTail())->getPrevious())->getValue(), pluralProperty)) {
                        suffixStr = u"’";
                    }
                }
            }
        }
        auto speakValue = inflection::dialog::DefaultArticleLookupFunction::getFeatureValue(valueConstraints, speakFeature);
        if (speakValue != nullptr) {
            valueConstraints[speakFeature] = *npc(speakValue) + suffixStr;
        }
        return displayString + suffixStr;
    }
    return displayString;
}

} // namespace inflection::grammar::synthesis
