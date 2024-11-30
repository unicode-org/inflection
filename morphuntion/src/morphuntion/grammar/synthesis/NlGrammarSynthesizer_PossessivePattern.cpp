/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_PossessivePattern.hpp>

#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_PossessivePattern::NlGrammarSynthesizer_PossessivePattern(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super(model)
{
    APOSTROPHES.freeze();
}

const ::std::map<::std::u16string_view, ::std::u16string_view>& NlGrammarSynthesizer_PossessivePattern::POSSESSIVE_PRONOUNS()
{
    static auto POSSESSIVE_PRONOUNS_ = new ::std::map<::std::u16string_view, ::std::u16string_view>( {
        {u"ik", u"mijn"},
        {u"me", u"mijn"},
        {u"mij", u"mijn"},
        {u"je", u"jouw"},
        {u"jij", u"jouw"},
        {u"jou", u"jouw"},
        {u"hij", u"zijn"},
        {u"hem", u"zijn"},
        // {u"ze", u"haar"},   // too little information: need count
        // {u"zij", u"haar"},  // too little information: need count
        {u"haar", u"haar"},
        // {u"we", u"ons"},    // too little information: need declension
        // {u"wij", u"ons"},   // too little information: need declension
        // {u"ons", u"ons"},   // too little information: need declension
        // {u"ze", u"hun"},   // too little information: need count
        // {u"zij", u"hun"},  // too little information: need count
        {u"hen", u"hun"},
        {u"hun", u"hun"},
    });
    return *npc(POSSESSIVE_PRONOUNS_);
}

const ::std::set<::std::u16string_view>& NlGrammarSynthesizer_PossessivePattern::COUNT_CONDITIONAL_PRONOUNS()
{
    static auto COUNT_CONDITIONAL_PRONOUNS_ = new ::std::set<::std::u16string_view>({u"ze", u"zij"});
    return *npc(COUNT_CONDITIONAL_PRONOUNS_);
}

const ::std::set<::std::u16string_view>& NlGrammarSynthesizer_PossessivePattern::DECLENSION_CONDITIONAL()
{
    static auto DECLENSION_CONDITIONAL_ = new ::std::set<::std::u16string_view>({u"we", u"wij", u"ons"});
    return *npc(DECLENSION_CONDITIONAL_);
}

bool NlGrammarSynthesizer_PossessivePattern::isSupported(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isValidCaseConstraint(constraints)) {
        logUnsupportedInflection(displayString, u"invalid constraints: expected case value");
        return false;
    }

    // Only support nominative --> genitive
    auto requestedCase(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    if (NlGrammarSynthesizer::getCase(&requestedCase) != NlGrammarSynthesizer::Case::genitive) {
        logUnsupportedInflection(displayString, u"Only support inflection for genitive case");
        return false;
    }
    if (COUNT_CONDITIONAL_PRONOUNS().find(displayString) != COUNT_CONDITIONAL_PRONOUNS().end() && !isValidCountConstraint(constraints)) {
        logUnsupportedInflection(displayString, u"invalid constraint: since input is ambiguous, count needs to be specified");
        return false;
    }
    if (DECLENSION_CONDITIONAL().find(displayString) != DECLENSION_CONDITIONAL().end() && !isValidDeclensionConstraint(constraints)) {
        logUnsupportedInflection(displayString, u"invalid constraint: since output is ambiguous, declension needs to be specified");
        return false;
    }
    return true;
}

::std::u16string NlGrammarSynthesizer_PossessivePattern::inflect(const ::std::u16string& displayString, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    if (!isSupported(displayString, constraints)) {
        return {};
    }
    auto possivePronoun = POSSESSIVE_PRONOUNS().find(displayString);
    if (possivePronoun  != POSSESSIVE_PRONOUNS().end()) {
        return ::std::u16string(possivePronoun->second);
    }

    if (COUNT_CONDITIONAL_PRONOUNS().find(displayString) != COUNT_CONDITIONAL_PRONOUNS().end()) {
        auto count(GrammarSynthesizerUtil::getFeatureValue(constraints, countFeature));
        switch (NlGrammarSynthesizer::getCount(&count)) {
            case NlGrammarSynthesizer::Count::singular: return u"haar";
            case NlGrammarSynthesizer::Count::plural: return u"hun";
            default: return {};
        }
    }

    if (DECLENSION_CONDITIONAL().find(displayString) != DECLENSION_CONDITIONAL().end()) {
        auto declension(GrammarSynthesizerUtil::getFeatureValue(constraints, declensionFeature));
        switch (NlGrammarSynthesizer::getDeclension(&declension)) {
            case NlGrammarSynthesizer::Declension::undeclined: return u"ons";
            case NlGrammarSynthesizer::Declension::declined: return u"onze";
            default: return {};
        }
    }

    // If not a pronoun, assume it's a name
    if (displayString.length() > 2) {
        if (isPossessive(displayString)) {
            return displayString;
        }
        auto last = displayString[displayString.length()-1];
        if (isApostrophe(last)) {
            return displayString + u"s";
        } else if (endsWithDutchSSound(displayString)) {
            return displayString + u"'";
        } else {
            return displayString + u"'s";
        }
    }
    return {};
}

bool NlGrammarSynthesizer_PossessivePattern::isApostrophe(char16_t c) const
{
    return APOSTROPHES.contains(c);
}

bool NlGrammarSynthesizer_PossessivePattern::isPossessive(const ::std::u16string& text) const
{
    if (text.length() > 2) {
        ::std::u16string lowerCaseText;
        ::morphuntion::util::StringViewUtils::lowercase(&lowerCaseText, text, ::morphuntion::util::LocaleUtils::DUTCH());
        auto secondToLast = lowerCaseText[lowerCaseText.length()-2];
        auto last = lowerCaseText[lowerCaseText.length()-1];

        // 's
        if (isApostrophe(secondToLast) && last == u's') {
            return true;
        }
        if (isApostrophe(last)) {
            return endsWithDutchSSound(lowerCaseText.substr(0, lowerCaseText.length()-1));
        }
    }
    return false;
}

bool NlGrammarSynthesizer_PossessivePattern::endsWithDutchSSound(const ::std::u16string& text) const
{
    ::std::u16string lowerCaseText;
    ::morphuntion::util::StringViewUtils::lowercase(&lowerCaseText, text, ::morphuntion::util::LocaleUtils::DUTCH());

    return ::morphuntion::util::StringViewUtils::endsWith(lowerCaseText, u"s")
        || ::morphuntion::util::StringViewUtils::endsWith(lowerCaseText, u"z")
        || ::morphuntion::util::StringViewUtils::endsWith(lowerCaseText, u"sch")
        || ::morphuntion::util::StringViewUtils::endsWith(lowerCaseText, u"x")
        || ::morphuntion::util::StringViewUtils::endsWith(lowerCaseText, u"sh");
}

} // namespace morphuntion::grammar::synthesis
