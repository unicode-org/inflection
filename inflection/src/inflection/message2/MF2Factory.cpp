/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
#include "inflection/message2/MF2Factory.hpp"

#include "inflection/message2/SemanticFeatureConceptArgument.hpp"
#include "inflection/dialog/CommonConceptFactory.hpp"
#include "inflection/dialog/InflectableStringConcept.hpp"
#include "inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp"
#include "inflection/dialog/NumberConcept.hpp"
#include "inflection/dialog/PronounConcept.hpp"
#include "inflection/dialog/SemanticConcept.hpp"
#include "inflection/dialog/SemanticConceptList.hpp"
#include "inflection/dialog/SemanticFeatureConceptBase.hpp"
#include "inflection/dialog/SemanticFeatureModel.hpp"
#include "inflection/dialog/SpeakableString.hpp"
#include "inflection/exception/ClassCastException.hpp"
#include "inflection/util/ArrayUtils.hpp"
#include "inflection/util/TypeConversionUtils.hpp"
#include "inflection/util/ULocale.hpp"
#include <memory>
#include <string_view>
#include <vector>
#include <unicode/locid.h>
#include <unicode/messageformat2.h>

using inflection::dialog::SpeakableString;
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

// Control / option names
static constexpr auto OPTION_TO                          = u"to";
static constexpr auto OPTION_WITH_VALUE                  = u"withValue";
static constexpr auto OPTION_WITH_STYLE                  = u"withStyle";
static constexpr auto OPTION_WITH_VARIANT                = u"withVariant";
static constexpr auto OPTION_WITH_TYPE                   = u"withType";
static constexpr auto OPTION_WITH_REFERENT               = u"withReferent";
static constexpr auto OPTION_WITH_BEFORE_FIRST           = u"withBeforeFirst";
static constexpr auto OPTION_WITH_AFTER_FIRST            = u"withAfterFirst";
static constexpr auto OPTION_WITH_ITEM_DELIMITER         = u"withItemDelimiter";
static constexpr auto OPTION_WITH_BEFORE_LAST            = u"withBeforeLast";
static constexpr auto OPTION_WITH_AFTER_LAST             = u"withAfterLast";
static constexpr auto OPTION_WITH_ITEM_PREFIX            = u"withItemPrefix";
static constexpr auto OPTION_WITH_ITEM_SUFFIX            = u"withItemSuffix";
static constexpr auto OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY = u"withAvoidItemAffixRedundancy";

// :numeral style values
static constexpr auto STYLE_AS_WORDS                     = u"asWords";
static constexpr auto STYLE_AS_SPOKEN_WORDS              = u"asSpokenWords";
static constexpr auto STYLE_AS_DIGITS                    = u"asDigits";
static constexpr auto STYLE_AS_ORDINAL_DIGITS            = u"asOrdinalDigits";

// :list withType values
static constexpr auto TYPE_AND                           = u"and";
static constexpr auto TYPE_OR                            = u"or";

// Bool option value
static constexpr auto VALUE_TRUE                         = u"true";

// Per-function fallback names passed to FormattedPlaceholder
static constexpr auto FN_INFLECTION                      = u"inflection";
static constexpr auto FN_NUMERAL                         = u"numeral";
static constexpr auto FN_QUANTIFY                        = u"quantify";
static constexpr auto FN_LIST                            = u"list";
static constexpr auto FN_PRONOUN                         = u"pronoun";

static UnicodeString xmlEscape(const std::u16string& s, bool attribute) {
    UnicodeString out;
    for (char16_t c : s) {
        switch (c) {
            case u'&': out += u"&amp;";
                break;
            case u'<': out += u"&lt;";
                break;
            case u'>': out += u"&gt;";
                break;
            case u'"': out += attribute ? UnicodeString(u"&quot;") : UnicodeString(c);
                break;
            default: out += c;
        }
    }
    return out;
}

static UnicodeString emit(const inflection::dialog::SpeakableString& s, MF2Factory::OutputMode mode) {
    if (mode == MF2Factory::OutputMode::PLAIN) {
        return s.getPrint();
    }
    if (s.getPrint() == s.getSpeak()) {
        return xmlEscape(s.getPrint(), false);
    }
    UnicodeString out(u"<sub alias=\"");
    out += xmlEscape(s.getSpeak(), true);
    out += u"\">";
    out += xmlEscape(s.getPrint(), false);
    out += u"</sub>";
    return out;
}

// String -> InflectableStringConcept (formatter model).
static std::unique_ptr<inflection::dialog::SemanticFeatureConceptBase>
conceptFromOperand(const Formattable& operand,
                   const inflection::dialog::SemanticFeatureModel* formatterModel,
                   UErrorCode& errorCode)
{
    if (operand.getType() == UFMT_STRING) {
        UnicodeString s = operand.getString(errorCode);
        return std::make_unique<inflection::dialog::InflectableStringConcept>(
            formatterModel, inflection::dialog::SpeakableString(s));
    }
    if (operand.getType() == UFMT_OBJECT) {
        const icu::message2::FormattableObject* obj = operand.getObject(errorCode);
        if (obj != nullptr && obj->tag() == UnicodeString(SemanticFeatureConceptArgument::TAG)) {
            const auto* arg = static_cast<const SemanticFeatureConceptArgument*>(obj);
            return std::unique_ptr<inflection::dialog::SemanticFeatureConceptBase>(arg->getConcept()->clone());
        }
        errorCode = U_MF_FORMATTING_ERROR;
        return nullptr;
    }
    errorCode = U_MF_FORMATTING_ERROR;
    return nullptr;
}

// Resolve a grammeme option value to a constraint string. A recognized grammeme value (or any
// value of an unbounded feature) is used directly; otherwise the value is treated as an operand
// (a word or a concept) and the requested feature is extracted from it for agreement. For
// example `number=$object` with $object="lights" constrains number to "plural", and `gender=$p`
// with a PronounConcept operand constrains gender to that pronoun's gender.
static std::u16string
resolveConstraintValue(const inflection::dialog::SemanticFeatureModel* model,
                       const inflection::dialog::SemanticFeature& feature,
                       const Formattable& value, UErrorCode& errorCode)
{
    if (value.getType() == UFMT_STRING) {
        UnicodeString s = value.getString(errorCode);
        std::u16string str(s.getBuffer(), static_cast<size_t>(s.length()));
        const auto& bounded = feature.getBoundedValues();
        if (feature.getType() != inflection::dialog::SemanticFeature::Type::BoundedValue
            || bounded.find(str) != bounded.end()) {
            return str;  // a recognized grammeme value, used directly
        }
        // Otherwise fall through: treat the string as a word to agree with.
    }
    auto operandConcept = conceptFromOperand(value, model, errorCode);
    if (operandConcept == nullptr) {
        return {};
    }
    std::unique_ptr<inflection::dialog::SpeakableString> featureValue(operandConcept->getFeatureValue(feature));
    return featureValue != nullptr ? featureValue->getPrint() : std::u16string();
}

class InflectionFormatterFactory : public FormatterFactory {
public:
    explicit InflectionFormatterFactory(MF2Factory::OutputMode mode) : mode(mode) {}
    Formatter* createFormatter(const Locale&, UErrorCode&) override;
private:
    MF2Factory::OutputMode mode;
};

class InflectionSelectorFactory : public SelectorFactory {
public:
    Selector* createSelector(const Locale&, UErrorCode&) const override;
};

icu::message2::FormatterFactory* MF2Factory::createInflectFormatterFactory(MF2Factory::OutputMode mode) {
    return new InflectionFormatterFactory(mode);
}

icu::message2::SelectorFactory* MF2Factory::createInflectSelectorFactory() {
    return new InflectionSelectorFactory();
}

const inflection::dialog::CommonConceptFactory* GetCommonConceptFactory(const Locale& locale) {
    return ::inflection::dialog::LocalizedCommonConceptFactoryProvider
        ::getDefaultCommonConceptFactoryProvider()
        ->getCommonConceptFactory(inflection::util::ULocale(locale.getName()));
}

const inflection::dialog::SemanticFeatureModel* GetSemanticFeatureModel(const Locale& locale) {
    return GetCommonConceptFactory(locale)->getSemanticFeatureModel();
}

class InflectionFormatter : public Formatter {
public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&& opts,
                                UErrorCode& errorCode) const override;
    InflectionFormatter(const inflection::dialog::SemanticFeatureModel* model,
                        MF2Factory::OutputMode mode)
        : model(model), mode(mode) {
    }
private:
     const ::inflection::dialog::SemanticFeatureModel* model;
     MF2Factory::OutputMode mode;
};

Formatter* InflectionFormatterFactory::createFormatter(const Locale& locale, UErrorCode& errorCode)
{
    if (U_FAILURE(errorCode)) {
        return nullptr;
    }

    Formatter* result = new InflectionFormatter(GetSemanticFeatureModel(locale), mode);
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

FormattedPlaceholder InflectionFormatter::format(
    FormattedPlaceholder&& arg, FunctionOptions&& options,
    UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return {};
    }

    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_INFLECTION);
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    UnicodeString result;

    UErrorCode conceptCode = U_ZERO_ERROR;
    auto conceptObj = conceptFromOperand(toFormat, model, conceptCode);
    if (conceptObj != nullptr) {
        UnicodeString inputString;
        if (toFormat.getType() == UFMT_STRING) {
            inputString = toFormat.getString(errorCode);
        }
        auto opt = options.getOptions();
        for (const auto& [key, value] : opt) {
            if (key == UnicodeString(OPTION_TO)) {
                continue;  // control option (the projection feature), not a grammeme constraint
            }
            auto constraint = conceptObj->getModel()->getFeature(key);
            if (constraint == nullptr) {
                // A bare option must name a known grammatical feature.
                errorCode = U_MF_FORMATTING_ERROR;
                return FormattedPlaceholder(FN_INFLECTION);
            }
            conceptObj->putConstraint(*constraint, resolveConstraintValue(model, *constraint, value, errorCode));
        }
        std::unique_ptr<inflection::dialog::SpeakableString> speakable;
        if (opt.contains(OPTION_TO)) {
            auto feature = conceptObj->getModel()->getFeature(opt.at(OPTION_TO).getString(errorCode));
            // Unknown to= feature: fall back to the uninflected form (intentional, not an error).
            if (feature != nullptr) {
                speakable.reset(conceptObj->getFeatureValue(*feature));
            }
        } else {
            speakable.reset(conceptObj->toSpeakableString());
        }
        if (speakable != nullptr) {
            result += emit(*speakable, mode);
        } else {
            // conceptObj return nullptr When the constrained values is an empty set
            // Fallback to the inputString, same as the non-string case.
            result += emit(inflection::dialog::SpeakableString(inputString), mode);
        }
    } else {
        result += toFormat.getString(errorCode);
    }

    return FormattedPlaceholder(arg, FormattedValue(std::move(result)));
}

class InflectionSelector : public Selector {
public:
    void selectKey(FormattedPlaceholder &&arg, FunctionOptions &&options,
                    const UnicodeString *keys, int32_t keysLen,
                    UnicodeString *prefs, int32_t &prefsLen,
                    UErrorCode &errorCode) const override;

    explicit InflectionSelector(const inflection::dialog::SemanticFeatureModel* model)
        : model(model) {
    }

private:
     const ::inflection::dialog::SemanticFeatureModel* model;
};

Selector* InflectionSelectorFactory::createSelector(
    const Locale& locale, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return nullptr;
    }

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
    if (U_FAILURE(errorCode)) {
        return;
    }
    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    prefsLen = 0;
    auto conceptObj = conceptFromOperand(toFormat, model, errorCode);
    if (conceptObj == nullptr) { errorCode = U_MF_SELECTOR_ERROR; return; }
    auto opt = options.getOptions();
    for (const auto& [key, value] : opt) {
        if (key == UnicodeString(OPTION_TO)) {
            continue;  // control option (the projection feature), not a grammeme constraint
        }
        auto feature = conceptObj->getModel()->getFeature(key);
        if (feature == nullptr) {
            // A bare option must name a known grammatical feature.
            errorCode = U_MF_SELECTOR_ERROR;
            return;
        }
        conceptObj->putConstraint(*feature, resolveConstraintValue(model, *feature, value, errorCode));
    }
    UnicodeString feature;
    if (opt.contains(OPTION_TO)) {
        auto value = conceptObj->getModel()->getFeature(opt.at(OPTION_TO).getString(errorCode));
        // Unknown to= feature: leave projected value empty — matches only catch-all (intentional, not an error).
        if (value != nullptr) {
            std::unique_ptr<inflection::dialog::SpeakableString> result(conceptObj->getFeatureValue(*value));
            if (result != nullptr) {
                feature = result->getPrint();
            }
        }
    } else {
        std::unique_ptr<inflection::dialog::SpeakableString> ss(conceptObj->toSpeakableString());
        if (ss != nullptr) {
            feature = ss->getPrint();
        }
    }
    for (int i = 0; i < keysLen; i++) {
        if (feature == keys[i]) {
            prefs[prefsLen++] = keys[i];
        }
    }
}

class NumeralFormatter : public Formatter {
public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&&,
                                UErrorCode&) const override;
    NumeralFormatter(const inflection::util::ULocale& locale, MF2Factory::OutputMode mode)
        : locale(locale), mode(mode) {}
private:
    inflection::util::ULocale locale;
    MF2Factory::OutputMode mode;
};

class NumeralFormatterFactory : public FormatterFactory {
public:
    explicit NumeralFormatterFactory(MF2Factory::OutputMode mode) : mode(mode) {}
    Formatter* createFormatter(const Locale& loc, UErrorCode& ec) override {
        if (U_FAILURE(ec)) {
            return nullptr;
        }
        return new NumeralFormatter(inflection::util::ULocale(loc.getName()), mode);
    }

    ~NumeralFormatterFactory() override;

private:
    MF2Factory::OutputMode mode;
};

NumeralFormatterFactory::~NumeralFormatterFactory() = default;

icu::message2::FormatterFactory* MF2Factory::createNumeralFormatterFactory(MF2Factory::OutputMode mode) {
    return new NumeralFormatterFactory(mode);
}

// Builds the SpeakableString for the numeral per style/variant (mirrors FormatTest::formatNumber).
static inflection::dialog::SpeakableString
numeralToSpeakable(const inflection::dialog::NumberConcept& num,
                   const std::u16string& style, const std::u16string& variant,
                   bool hasVariant, UErrorCode& errorCode) {
    if (style.empty()) {
        std::unique_ptr<SpeakableString> s(num.toSpeakableString());
        return *s;
    }
    if (style == STYLE_AS_WORDS) {
        return SpeakableString(hasVariant ? num.asWords(variant) : num.getAsWords());
    }
    if (style == STYLE_AS_SPOKEN_WORDS) {
        return hasVariant ? num.asSpokenWords(variant) : num.getAsSpokenWords();
    }
    if (style == STYLE_AS_DIGITS) {
        if (hasVariant) {
            return SpeakableString(num.asDigits(variant));
        }
        return num.getAsDigits();
    }
    if (style == STYLE_AS_ORDINAL_DIGITS) {
        return SpeakableString(hasVariant ? num.asDigits(variant) : num.getAsOrdinalDigits());
    }
    errorCode = U_MF_FORMATTING_ERROR;
    return SpeakableString(u"");
}

FormattedPlaceholder NumeralFormatter::format(
    FormattedPlaceholder&& arg, FunctionOptions&& options,
    UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return {};
    }
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_NUMERAL);
    }
    const Formattable &toFormat = arg.asFormattable();

    std::unique_ptr<inflection::dialog::NumberConcept> num;
    if (toFormat.getType() == UFMT_DOUBLE) {
        num = std::make_unique<inflection::dialog::NumberConcept>(
            toFormat.getDouble(errorCode), locale, locale);
    }
    else {
        num = std::make_unique<inflection::dialog::NumberConcept>(
            toFormat.getInt64(errorCode), locale, locale);
    }

    std::u16string style;
    std::u16string variant;
    bool hasVariant = false;
    auto opt = options.getOptions();
    if (opt.contains(OPTION_WITH_STYLE)) {
        const UnicodeString &v = opt.at(OPTION_WITH_STYLE).getString(errorCode);
        style = std::u16string(v.getBuffer(), v.length());
    }
    if (opt.contains(OPTION_WITH_VARIANT)) {
        const UnicodeString &v = opt.at(OPTION_WITH_VARIANT).getString(errorCode);
        variant = std::u16string(v.getBuffer(), v.length());
        hasVariant = true;
    }

    inflection::dialog::SpeakableString result =
            numeralToSpeakable(*num, style, variant, hasVariant, errorCode);
    if (U_FAILURE(errorCode)) {
        return FormattedPlaceholder(FN_NUMERAL);
    }
    UnicodeString out = emit(result, mode);
    return FormattedPlaceholder(arg, FormattedValue(std::move(out)));
}

class QuantifyFormatter : public Formatter {
public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&&, UErrorCode&) const override;
    QuantifyFormatter(const inflection::dialog::SemanticFeatureModel* model,
                      const inflection::dialog::CommonConceptFactory* ccf,
                      const inflection::util::ULocale& locale, MF2Factory::OutputMode mode)
        : model(model), ccf(ccf), locale(locale), mode(mode) {}
private:
    const inflection::dialog::SemanticFeatureModel* model;
    const inflection::dialog::CommonConceptFactory* ccf;
    inflection::util::ULocale locale;
    MF2Factory::OutputMode mode;
};

class QuantifyFormatterFactory : public FormatterFactory {
public:
    explicit QuantifyFormatterFactory(MF2Factory::OutputMode mode) : mode(mode) {}
    Formatter* createFormatter(const Locale&, UErrorCode&) override;
private:
    MF2Factory::OutputMode mode;
};

Formatter* QuantifyFormatterFactory::createFormatter(const Locale& loc, UErrorCode& ec) {
    if (U_FAILURE(ec)) {
        return nullptr;
    }
    return new QuantifyFormatter(GetSemanticFeatureModel(loc), GetCommonConceptFactory(loc),
                                 inflection::util::ULocale(loc.getName()), mode);
}

icu::message2::FormatterFactory* MF2Factory::createQuantifyFormatterFactory(OutputMode mode) {
    return new QuantifyFormatterFactory(mode);
}

FormattedPlaceholder QuantifyFormatter::format(FormattedPlaceholder&& arg, FunctionOptions&& options, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return {};
    }
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_QUANTIFY);
    }
    std::unique_ptr<inflection::dialog::SemanticFeatureConceptBase> unit =
            conceptFromOperand(arg.asFormattable(), model, errorCode);
    if (unit == nullptr) {
        return FormattedPlaceholder(FN_QUANTIFY);
    }

    auto opt = options.getOptions();
    if (!opt.contains(OPTION_WITH_VALUE)) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_QUANTIFY);
    }
    if (opt.contains(OPTION_WITH_STYLE) || opt.contains(OPTION_WITH_VARIANT)) {
        // reserved, not implemented, but perhaps it will be used in the future.
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_QUANTIFY);
    }

    const Formattable &valueF = opt.at(OPTION_WITH_VALUE);
    std::unique_ptr<inflection::dialog::NumberConcept> num;
    if (valueF.getType() == UFMT_DOUBLE) {
        num = std::make_unique<inflection::dialog::NumberConcept>(valueF.getDouble(errorCode), locale, locale);
    }
    else {
        // Integer-valued args (UFMT_INT64/UFMT_LONG) take this branch; only genuine doubles take the branch above.
        num = std::make_unique<inflection::dialog::NumberConcept>(valueF.getInt64(errorCode), locale, locale);
    }

    for (const auto &entry: opt) {
        const UnicodeString &key = entry.first;
        // Control options (exact allow-list, NOT a with-prefix test: some real
        // semantic features are with-prefixed). withValue consumed above;
        // withStyle/withVariant rejected above.
        if (key == UnicodeString(OPTION_WITH_VALUE) || key == UnicodeString(OPTION_WITH_STYLE)
            || key == UnicodeString(OPTION_WITH_VARIANT)) {
            continue;
        }
        std::u16string name(key.getBuffer(), key.length());
        const auto *feature = unit->getModel()->getFeature(name);
        if (feature == nullptr) {
            // unknown feature (mirror FormatTest strictness)
            errorCode = U_MF_FORMATTING_ERROR;
            return FormattedPlaceholder(FN_QUANTIFY);
        }
        const UnicodeString &v = entry.second.getString(errorCode);
        unit->putConstraint(*feature, std::u16string(v.getBuffer(), v.length()));
    }

    std::unique_ptr<inflection::dialog::SpeakableString> result(ccf->quantify(*num, unit.get()));
    if (result == nullptr) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_QUANTIFY);
    }
    UnicodeString out = emit(*result, mode);
    return FormattedPlaceholder(arg, FormattedValue(std::move(out)));
}

class ListFormatter : public Formatter {
public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&&, UErrorCode&) const override;
    ListFormatter(const inflection::dialog::SemanticFeatureModel* model,
                  const inflection::dialog::CommonConceptFactory* ccf, MF2Factory::OutputMode mode)
        : model(model), ccf(ccf), mode(mode) {}
private:
    const inflection::dialog::SemanticFeatureModel* model;
    const inflection::dialog::CommonConceptFactory* ccf;
    MF2Factory::OutputMode mode;
};

class ListFormatterFactory : public FormatterFactory {
public:
    explicit ListFormatterFactory(MF2Factory::OutputMode mode) : mode(mode) {
    }

    Formatter *createFormatter(const Locale &, UErrorCode &) override;

private:
    MF2Factory::OutputMode mode;
};

Formatter *ListFormatterFactory::createFormatter(const Locale &loc, UErrorCode &ec) {
    if (U_FAILURE(ec)) {
        return nullptr;
    }
    return new ListFormatter(GetSemanticFeatureModel(loc), GetCommonConceptFactory(loc), mode);
}

icu::message2::FormatterFactory *MF2Factory::createListFormatterFactory(OutputMode mode) {
    return new ListFormatterFactory(mode);
}

FormattedPlaceholder ListFormatter::format(FormattedPlaceholder&& arg, FunctionOptions&& options, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return {};
    }
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_LIST);
    }
    const Formattable& arrF = arg.asFormattable();
    if (arrF.getType() != UFMT_ARRAY) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_LIST);
    }
    int32_t count = 0;
    const Formattable* elems = arrF.getArray(count, errorCode);
    if (U_FAILURE(errorCode)) {
        return FormattedPlaceholder(FN_LIST);
    }

    auto opt = options.getOptions();

    std::vector<std::unique_ptr<inflection::dialog::InflectableStringConcept> > items;
    std::vector<const inflection::dialog::SemanticFeatureConceptBase*> itemPtrs;
    for (int32_t i = 0; i < count; i++) {
        UnicodeString elemStr = elems[i].getString(errorCode);
        if (U_FAILURE(errorCode)) {
            return FormattedPlaceholder(FN_LIST);
        }
        items.push_back(std::make_unique<inflection::dialog::InflectableStringConcept>(
            model, inflection::dialog::SpeakableString(elemStr)));
        itemPtrs.push_back(items.back().get());
    }

    std::unique_ptr<inflection::dialog::SemanticConceptList> list;
    if (!opt.contains(OPTION_WITH_TYPE)) {
        list = std::make_unique<inflection::dialog::SemanticConceptList>(model, itemPtrs);
    }
    else {
        const UnicodeString& typeV = opt.at(OPTION_WITH_TYPE).getString(errorCode);
        std::u16string typeVal(typeV.getBuffer(), typeV.length());
        if (typeVal == TYPE_AND) {
            list.reset(ccf->createAndList(itemPtrs));
        }
        else if (typeVal == TYPE_OR) {
            list.reset(ccf->createOrList(itemPtrs));
        }
        else {
            errorCode = U_MF_FORMATTING_ERROR;
            return FormattedPlaceholder(FN_LIST);
        }
    }
    if (list == nullptr) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_LIST);
    }

    // Apply separator-setter options.
    if (opt.contains(OPTION_WITH_BEFORE_FIRST)) {
        list->setBeforeFirst(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_BEFORE_FIRST).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_AFTER_FIRST)) {
        list->setAfterFirst(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_AFTER_FIRST).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_ITEM_DELIMITER)) {
        list->setItemDelimiter(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_DELIMITER).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_BEFORE_LAST)) {
        list->setBeforeLast(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_BEFORE_LAST).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_AFTER_LAST)) {
        list->setAfterLast(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_AFTER_LAST).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_ITEM_PREFIX)) {
        list->setItemPrefix(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_PREFIX).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_ITEM_SUFFIX)) {
        list->setItemSuffix(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_SUFFIX).getString(errorCode)));
    }
    if (opt.contains(OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY)) {
        const UnicodeString& v = opt.at(OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY).getString(errorCode);
        list->setAvoidItemAffixRedundancy(std::u16string(v.getBuffer(), static_cast<size_t>(v.length())) == VALUE_TRUE);
    }
    // Apply grammeme constraints (options not in the control allow-list).
    static constexpr std::u16string_view LIST_CONTROL[] = {
        // This must be in sorted order.
        OPTION_WITH_AFTER_FIRST,
        OPTION_WITH_AFTER_LAST,
        OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY,
        OPTION_WITH_BEFORE_FIRST,
        OPTION_WITH_BEFORE_LAST,
        OPTION_WITH_ITEM_DELIMITER,
        OPTION_WITH_ITEM_PREFIX,
        OPTION_WITH_ITEM_SUFFIX,
        OPTION_WITH_TYPE,
    };
    for (const auto& entry : opt) {
        std::u16string_view nameView(entry.first.getBuffer(), static_cast<size_t>(entry.first.length()));
        if (inflection::util::ArrayUtils::contains<LIST_CONTROL>(nameView)) { // control options (exact allow-list)
            continue;
        }
        const auto* feature = model->getFeature(nameView);
        if (feature == nullptr) {
            // unknown feature (mirror FormatTest/:quantify strictness)
            errorCode = U_MF_FORMATTING_ERROR;
            return FormattedPlaceholder(FN_LIST);
        }
        const UnicodeString& v = entry.second.getString(errorCode);
        list->putConstraint(*feature, std::u16string(v.getBuffer(), v.length()));
    }

    std::unique_ptr<inflection::dialog::SpeakableString> result(list->toSpeakableString());
    if (result == nullptr) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_LIST);
    }
    UnicodeString out = emit(*result, mode);
    return FormattedPlaceholder(arg, FormattedValue(std::move(out)));
}

// Operand -> PronounConcept. String = a pronoun seed (PronounConcept(model, seed) THROWS on a bad
// seed, so it is caught and mapped).
static std::unique_ptr<inflection::dialog::PronounConcept>
pronounFromOperand(const Formattable& operand,
                   const inflection::dialog::SemanticFeatureModel* formatterModel, UErrorCode& errorCode) {
    if (operand.getType() == UFMT_STRING) {
        UnicodeString s = operand.getString(errorCode);
        if (U_FAILURE(errorCode)) {
            return nullptr;
        }
        try {
            return std::make_unique<inflection::dialog::PronounConcept>(
                *formatterModel, std::u16string_view(s.getBuffer(), static_cast<size_t>(s.length())));
        } catch (const std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, &errorCode);
            return nullptr;
        }
    }
    if (operand.getType() == UFMT_OBJECT) {
        const icu::message2::FormattableObject* obj = operand.getObject(errorCode);
        if (obj != nullptr && obj->tag() == UnicodeString(SemanticFeatureConceptArgument::TAG)) {
            const auto* arg = static_cast<const SemanticFeatureConceptArgument*>(obj);
            try {
                // The carried concept must be a PronounConcept for :pronoun.
                const auto* pronoun = inflection::exception::ClassCastException::verifyCast<
                    const inflection::dialog::PronounConcept*>(arg->getConcept());
                return std::unique_ptr<inflection::dialog::PronounConcept>(pronoun->clone());
            } catch (const std::exception& e) {
                inflection::util::TypeConversionUtils::convert(e, &errorCode);
                return nullptr;
            }
        }
        errorCode = U_MF_FORMATTING_ERROR;
        return nullptr;
    }
    errorCode = U_MF_FORMATTING_ERROR;
    return nullptr;
}

class PronounFormatter : public Formatter {
public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&&, UErrorCode&) const override;

    PronounFormatter(const inflection::dialog::SemanticFeatureModel* model, MF2Factory::OutputMode mode)
        : model(model), mode(mode) {
    }

private:
    const inflection::dialog::SemanticFeatureModel* model{};
    MF2Factory::OutputMode mode;
};

class PronounFormatterFactory : public FormatterFactory {
public:
    explicit PronounFormatterFactory(MF2Factory::OutputMode mode) : mode(mode) {
    }

    Formatter* createFormatter(const Locale&, UErrorCode&) override;

private:
    MF2Factory::OutputMode mode;
};

class PronounSelector : public Selector {
public:
    void selectKey(FormattedPlaceholder&&, FunctionOptions&&, const UnicodeString*, int32_t,
                   UnicodeString*, int32_t&, UErrorCode&) const override;

    explicit PronounSelector(const inflection::dialog::SemanticFeatureModel* model) : model(model) {
    }

private:
    const inflection::dialog::SemanticFeatureModel* model;
};

class PronounSelectorFactory : public SelectorFactory {
public:
    Selector* createSelector(const Locale&, UErrorCode&) const override;
};

Formatter* PronounFormatterFactory::createFormatter(const Locale& loc, UErrorCode& ec) {
    if (U_FAILURE(ec)) {
        return nullptr;
    }
    return new PronounFormatter(GetSemanticFeatureModel(loc), mode);
}

Selector* PronounSelectorFactory::createSelector(const Locale& loc, UErrorCode& ec) const {
    if (U_FAILURE(ec)) {
        return nullptr;
    }
    return new PronounSelector(GetSemanticFeatureModel(loc));
}

icu::message2::FormatterFactory* MF2Factory::createPronounFormatterFactory(OutputMode mode) {
    return new PronounFormatterFactory(mode);
}

icu::message2::SelectorFactory* MF2Factory::createPronounSelectorFactory() {
    return new PronounSelectorFactory();
}

FormattedPlaceholder PronounFormatter::format(FormattedPlaceholder&& arg, FunctionOptions&& options,
                                              UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return {};
    }
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_PRONOUN);
    }
    auto pronoun = pronounFromOperand(arg.asFormattable(), model, errorCode);
    if (pronoun == nullptr) {
        return FormattedPlaceholder(FN_PRONOUN);
    }

    auto opt = options.getOptions();
    if (opt.contains(OPTION_TO) && opt.contains(OPTION_WITH_REFERENT)) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder(FN_PRONOUN);
    }
    for (const auto& [key, value] : opt) {
        if (key == UnicodeString(OPTION_TO) || key == UnicodeString(OPTION_WITH_REFERENT)) {
            continue;
        }
        auto feature = pronoun->getModel()->getFeature(key);
        if (feature == nullptr) {
            errorCode = U_MF_FORMATTING_ERROR;
            return FormattedPlaceholder(FN_PRONOUN);
        }
        pronoun->putConstraint(*feature, resolveConstraintValue(model, *feature, value, errorCode));
    }

    std::unique_ptr<inflection::dialog::SpeakableString> speakable;
    if (opt.contains(OPTION_TO)) {
        auto feature = pronoun->getModel()->getFeature(opt.at(OPTION_TO).getString(errorCode));
        // Unknown to= feature: leave null -> fallback (intentional, not an error).
        if (feature != nullptr) {
            speakable.reset(pronoun->getFeatureValue(*feature));
        }
    }
    else if (opt.contains(OPTION_WITH_REFERENT)) {
        auto referent = conceptFromOperand(opt.at(OPTION_WITH_REFERENT), model, errorCode);
        if (referent == nullptr) {
            return FormattedPlaceholder(FN_PRONOUN);
        }
        speakable.reset(pronoun->toSpeakableString(*referent));
    }
    else {
        speakable.reset(pronoun->toSpeakableString());
    }

    UnicodeString result;
    if (speakable != nullptr) {
        result += emit(*speakable, mode);
    }
    return FormattedPlaceholder(arg, FormattedValue(std::move(result)));
}

void PronounSelector::selectKey(FormattedPlaceholder&& arg, FunctionOptions&& options,
                                const UnicodeString* keys, int32_t keysLen,
                                UnicodeString* prefs, int32_t& prefsLen, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return;
    }
    if (!arg.canFormat()) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }
    prefsLen = 0;
    auto pronoun = pronounFromOperand(arg.asFormattable(), model, errorCode);
    if (pronoun == nullptr) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }

    auto opt = options.getOptions();
    if (opt.contains(OPTION_TO) && opt.contains(OPTION_WITH_REFERENT)) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }
    for (const auto& [key, value] : opt) {
        if (key == UnicodeString(OPTION_TO) || key == UnicodeString(OPTION_WITH_REFERENT)) {
            continue;
        }
        auto feature = pronoun->getModel()->getFeature(key);
        if (feature == nullptr) {
            errorCode = U_MF_SELECTOR_ERROR;
            return;
        }
        pronoun->putConstraint(*feature, resolveConstraintValue(model, *feature, value, errorCode));
    }

    UnicodeString projected;
    std::unique_ptr<inflection::dialog::SpeakableString> v;
    if (opt.contains(OPTION_TO)) {
        auto feature = pronoun->getModel()->getFeature(opt.at(OPTION_TO).getString(errorCode));
        if (feature != nullptr) {
            v.reset(pronoun->getFeatureValue(*feature));
        }
    }
    else if (opt.contains(OPTION_WITH_REFERENT)) {
        auto referent = conceptFromOperand(opt.at(OPTION_WITH_REFERENT), model, errorCode);
        if (referent == nullptr) {
            errorCode = U_MF_SELECTOR_ERROR;
            return;
        }
        v.reset(pronoun->toSpeakableString(*referent));
    }
    else {
        v.reset(pronoun->toSpeakableString());
    }
    if (v != nullptr) {
        projected = v->getPrint();
    }
    for (int i = 0; i < keysLen; i++) {
        if (projected == keys[i]) {
            prefs[prefsLen++] = keys[i];
        }
    }
}

}  // namespace inflection::message2
