/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include "morphuntion/tokenizer/Tokenizer.hpp"
#include <memory>
#include <string_view>

class morphuntion::grammar::synthesis::TrGrammarSynthesizer_TrDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dictionary::DictionaryMetaData& dictionary;
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    const ::morphuntion::dialog::SemanticFeature& caseFeature;
    const ::morphuntion::dialog::SemanticFeature& countFeature;
    const ::morphuntion::dialog::SemanticFeature& personFeature;
    const ::morphuntion::dialog::SemanticFeature& pronounFeature;
    const ::morphuntion::dialog::SemanticFeature& pronounNumberFeature;
    const ::morphuntion::dialog::SemanticFeature& copulaFeature;
    const ::morphuntion::dialog::SemanticFeature& tenseFeature;

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue *getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

private:
    static ::std::u16string addPossessiveSuffixes(TrGrammarSynthesizer::Count countValue, TrGrammarSynthesizer::Person personValue, TrGrammarSynthesizer::Pronoun pronounValue, bool isPossessiveCompound, TrGrammarSynthesizer::VowelGroup &vowGroup, const ::std::u16string& lastPart);
    static std::u16string addCaseSuffixes(
            morphuntion::grammar::synthesis::TrGrammarSynthesizer::Case caseValue, bool isPossessive,
            morphuntion::grammar::synthesis::TrGrammarSynthesizer::VowelGroup &vowGroup, const std::u16string &lastPart);
    static ::std::u16string addCopulaSuffixes(
            morphuntion::grammar::synthesis::TrGrammarSynthesizer::Copula copulaValue,
            morphuntion::grammar::synthesis::TrGrammarSynthesizer::Tense tenseValue,
            morphuntion::grammar::synthesis::TrGrammarSynthesizer::VowelGroup vowGroup,
            const std::u16string &lastPart);
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue* generateDisplayValue(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& formConstraints, const ::std::u16string& displayStringNormalized, ::std::u16string* suffixString, bool endsWithNumber, bool isForeign, bool isException) const;
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
    explicit TrGrammarSynthesizer_TrDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    TrGrammarSynthesizer_TrDisplayFunction(TrGrammarSynthesizer_TrDisplayFunction&) = delete;
    ~TrGrammarSynthesizer_TrDisplayFunction() override;


private:
    static const ::icu4cxx::UnicodeSet& DEFAULT_DISCONT_HARD_CONSONANTS_END();
    static const ::icu4cxx::UnicodeSet& DEFAULT_HARD_CONSONANTS_END();

private:
    friend class TrGrammarSynthesizer_InterrogativeArticleLookupFunction;
    friend class TrGrammarSynthesizer_DeConjunctionArticleLookupFunction;
};
