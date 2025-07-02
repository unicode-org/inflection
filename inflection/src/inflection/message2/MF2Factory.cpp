/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include "inflection/message2/MF2Factory.hpp"

#include "inflection/dialog/InflectableStringConcept.hpp"
#include "inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp"
#include "inflection/dialog/SemanticFeatureModel.hpp"
#include "inflection/dialog/SpeakableString.hpp"
#include "inflection/lang/features/LanguageGrammarFeatures.hpp"
#include "inflection/util/ULocale.hpp"

#include <unicode/locid.h>
#include <unicode/messageformat2.h>
#include <unicode/messageformat2_formattable.h>

using U_ICU_NAMESPACE::Locale;
using U_ICU_NAMESPACE::UnicodeString;
using U_ICU_NAMESPACE::message2::Formatter;
using U_ICU_NAMESPACE::message2::Formattable;
using U_ICU_NAMESPACE::message2::FormattedValue;
using U_ICU_NAMESPACE::message2::FormattedPlaceholder;
using U_ICU_NAMESPACE::message2::FormatterFactory;
using U_ICU_NAMESPACE::message2::FunctionOptions;
using U_ICU_NAMESPACE::message2::FunctionOptionsMap;
using U_ICU_NAMESPACE::message2::MessageArguments;
using U_ICU_NAMESPACE::message2::MessageFormatter;
using U_ICU_NAMESPACE::message2::MFFunctionRegistry;
using U_ICU_NAMESPACE::message2::Selector;
using U_ICU_NAMESPACE::message2::SelectorFactory;

namespace inflection::message2 {

class InflectionFormatterFactory : public FormatterFactory {
    public:
     Formatter* createFormatter(const Locale&, UErrorCode&) override;
};

class InflectionSelectorFactory : public SelectorFactory {
    public:
     Selector* createSelector(const Locale&, UErrorCode&) const override;
};

icu::message2::FormatterFactory* MF2Factory::CreateFormatterFactory() {
    return new InflectionFormatterFactory();
}

icu::message2::SelectorFactory* MF2Factory::CreateSelectorFactory() {
    return new InflectionSelectorFactory();
}

const inflection::dialog::SemanticFeatureModel* GetSemanticFeatureModel(
    const Locale& locale) {
    return ::inflection::dialog::LocalizedCommonConceptFactoryProvider
        ::getDefaultCommonConceptFactoryProvider()
        ->getCommonConceptFactory(
            inflection::util::ULocale(locale.getName()))
        ->getSemanticFeatureModel();
}

class InflectionFormatter : public Formatter {
  public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&& opts,
                                UErrorCode& errorCode) const override;
    InflectionFormatter(const inflection::dialog::SemanticFeatureModel* model)
        : model(model) {
    }
  private:
     const ::inflection::dialog::SemanticFeatureModel* model;
};

Formatter* InflectionFormatterFactory::createFormatter(
    const Locale& locale, UErrorCode& errorCode) {
    if (U_FAILURE(errorCode)) { return nullptr; }

    Formatter* result = new InflectionFormatter(GetSemanticFeatureModel(locale));
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

FormattedPlaceholder InflectionFormatter::format(
    FormattedPlaceholder&& arg, FunctionOptions&& options,
    UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) { return {}; }

    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder("inflection");
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    UnicodeString result;

    switch (toFormat.getType()) {
        case UFMT_STRING: {
            UnicodeString inputString = toFormat.getString(errorCode);
            inflection::dialog::SpeakableString input(inputString);
            inflection::dialog::InflectableStringConcept stringConcept(
                model, input);
            for (const auto& [key, value] : options.getOptions()) {
                auto constraint = model->getFeature(key);
                if (constraint != nullptr) {
                    stringConcept.putConstraint(*constraint,
                                                value.getString(errorCode));
                }
            }
            std::unique_ptr<inflection::dialog::SpeakableString> string(
                stringConcept.toSpeakableString());
            if (string != nullptr) {
                result += string->getPrint();
            } else {
                // stringConcept return nullptr When the constrained values is an empty set
                // Fallback to the inputString, same as the default case in switch().
                result += inputString;
            }
            break;
        }
        default: {
            result += toFormat.getString(errorCode);
            break;
        }
    }

    return FormattedPlaceholder(arg, FormattedValue(std::move(result)));
}

class InflectionSelector : public Selector {
  public:
    void selectKey(FormattedPlaceholder &&arg, FunctionOptions &&options,
                    const UnicodeString *keys, int32_t keysLen,
                    UnicodeString *prefs, int32_t &prefsLen,
                    UErrorCode &status) const override;

     InflectionSelector(const inflection::dialog::SemanticFeatureModel* model)
         : model(model) {
     }

  private:
     const ::inflection::dialog::SemanticFeatureModel* model;
};

Selector* InflectionSelectorFactory::createSelector(
    const Locale& locale, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) { return nullptr; }

    Selector* result = new InflectionSelector(GetSemanticFeatureModel(locale));
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

void InflectionSelector::selectKey(
    FormattedPlaceholder &&arg, FunctionOptions &&options,
    const UnicodeString *keys, int32_t keysLen,
    UnicodeString *prefs, int32_t &prefsLen, UErrorCode &errorCode) const {
    if (U_FAILURE(errorCode)) { return; }
    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    prefsLen = 0;
    auto opt = options.getOptions();
    if (toFormat.getType() == UFMT_STRING) {
        inflection::dialog::SpeakableString input(
            toFormat.getString(errorCode));
        inflection::dialog::InflectableStringConcept stringConcept(
            model, input);
        if (!opt.contains(u"select")) {
            errorCode = U_MF_SELECTOR_ERROR;
            return;
        }
        for (const auto& [key, value] : options.getOptions()) {
            auto constraint = model->getFeature(key);
            if (constraint != nullptr) {
                stringConcept.putConstraint(*constraint,
                                            value.getString(errorCode));
            }
        }
        auto value = model->getFeature(opt.at(u"select").getString(errorCode));
        UnicodeString feature;
        if (value != nullptr) {
            std::unique_ptr<inflection::dialog::SpeakableString> result(
                stringConcept.getFeatureValue(*value));
            if (result != nullptr) {
                feature = result->getPrint();
            }
        }
        for (int i = 0; i < keysLen; i++) {
            if (feature == keys[i]) {
                prefs[prefsLen++] = keys[i];
            }
        }
    }
    return;
}

}  // namespace inflection::message2
