/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <memory>
#include <string_view>

class inflection::grammar::synthesis::TrGrammarSynthesizer_TrDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& personFeature;
    const ::inflection::dialog::SemanticFeature& pronounFeature;
    const ::inflection::dialog::SemanticFeature& pronounNumberFeature;
    const ::inflection::dialog::SemanticFeature& copulaFeature;
    const ::inflection::dialog::SemanticFeature& tenseFeature;
    ::std::u16string_view endingSingleQuote { u"'’" };
    ::std::u16string_view endingDoubleQuote { u"\"”" };

public:
    ::inflection::dialog::DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string addPossessiveSuffixes(TrGrammarSynthesizer::Number numberValue, TrGrammarSynthesizer::Person personValue, TrGrammarSynthesizer::Pronoun pronounValue, bool isPossessiveCompound, TrGrammarSynthesizer::VowelGroup &vowGroup, const ::std::u16string& lastPart);
    static std::u16string addCaseSuffixes(
            inflection::grammar::synthesis::TrGrammarSynthesizer::Case caseValue, bool isPossessive,
            inflection::grammar::synthesis::TrGrammarSynthesizer::VowelGroup &vowGroup, const std::u16string &lastPart);
    static ::std::u16string addCopulaSuffixes(
            inflection::grammar::synthesis::TrGrammarSynthesizer::Copula copulaValue,
            inflection::grammar::synthesis::TrGrammarSynthesizer::Tense tenseValue,
            inflection::grammar::synthesis::TrGrammarSynthesizer::VowelGroup vowGroup,
            const std::u16string &lastPart);
    ::inflection::dialog::DisplayValue* generateDisplayValue(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& formConstraints, const ::std::u16string& displayStringNormalized, ::std::u16string* suffixString, bool endsWithNumber, bool isForeign, bool isException) const;
    ::std::u16string normalizeString(const ::std::u16string& word) const;
    char16_t getLastLetter(const ::std::u16string& word) const;
    static bool endsWithVowel(const ::std::u16string& word);
    static bool startsWithVowel(const ::std::u16string& word);
    static ::std::u16string getLastWord(const ::std::u16string& str);
    static bool endsWithHardConsonant(::std::u16string_view word, bool discontinuous);
    ::std::u16string getPossessiveCompoundSuffix(bool isDisplayMultiWord, const ::std::u16string& word) const;
    bool isOneToken(const ::std::u16string& word) const;
    char16_t getLastVowel(const ::std::u16string& word) const;
    static ::std::u16string assimilateLastConsonant(const ::std::u16string& word, bool oneWord);
    static ::std::u16string revertLastConsonantAssimilation(const ::std::u16string& word);
    static ::std::u16string assimilateFirstConsonant(const ::std::u16string& word);
    int64_t frontRoundProperty {  };
    int64_t frontUnroundProperty {  };
    int64_t backRoundProperty {  };
    int64_t backUnroundProperty {  };
    int64_t foreignProperty {  };
    int64_t exceptionProperty {  };
    int64_t compoundProperty {  };
    int64_t softConsonantProperty {  };
    int64_t hardConsonantProperty {  };
    TrGrammarSynthesizer::VowelGroup getVowelGroup(::std::u16string_view str) const;
    bool isForeignWord(::std::u16string_view str) const;
    bool isExceptionWord(::std::u16string_view str) const;
    bool isCompoundWord(::std::u16string_view str) const;
    bool doesEndWithSoftConsonant(::std::u16string_view str) const;
    bool doesEndWithHardConsonant(::std::u16string_view str) const;
    static bool isOneVowelWord(::std::u16string_view word);
    static bool endsWithNumber(const ::std::u16string& word);
    static ::std::u16string trimEnd(const ::std::u16string& word);

public: /* package */
    explicit TrGrammarSynthesizer_TrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    TrGrammarSynthesizer_TrDisplayFunction(TrGrammarSynthesizer_TrDisplayFunction&) = delete;
    ~TrGrammarSynthesizer_TrDisplayFunction() override;
    TrGrammarSynthesizer_TrDisplayFunction(const TrGrammarSynthesizer_TrDisplayFunction&) = delete;
    TrGrammarSynthesizer_TrDisplayFunction& operator=(const TrGrammarSynthesizer_TrDisplayFunction&) = delete;

private:
    static const ::icu4cxx::UnicodeSet& DEFAULT_DISCONT_HARD_CONSONANTS_END();
    static const ::icu4cxx::UnicodeSet& DEFAULT_HARD_CONSONANTS_END();

private:
    friend class TrGrammarSynthesizer_InterrogativeArticleLookupFunction;
    friend class TrGrammarSynthesizer_DeConjunctionArticleLookupFunction;
};
