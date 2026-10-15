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
using U_ICU_NAMESPACE::LocalPointer;
using U_ICU_NAMESPACE::UnicodeString;
using U_ICU_NAMESPACE::message2::Formattable;
using U_ICU_NAMESPACE::message2::Function;
using U_ICU_NAMESPACE::message2::FunctionContext;
using U_ICU_NAMESPACE::message2::FunctionOptions;
using U_ICU_NAMESPACE::message2::FunctionOptionsMap;
using U_ICU_NAMESPACE::message2::FunctionValue;
using U_ICU_NAMESPACE::message2::MessageArguments;
using U_ICU_NAMESPACE::message2::MessageFormatter;
using U_ICU_NAMESPACE::message2::MFFunctionRegistry;

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

const inflection::dialog::CommonConceptFactory* GetCommonConceptFactory(const Locale& locale)
{
    return ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()->getCommonConceptFactory(
        inflection::util::ULocale(locale.getName()));
}

const inflection::dialog::SemanticFeatureModel* GetSemanticFeatureModel(const Locale& locale)
{
    return GetCommonConceptFactory(locale)->getSemanticFeatureModel();
}

// Builds the SpeakableString for the numeral per style/variant (mirrors FormatTest::formatNumber).
static inflection::dialog::SpeakableString numeralToSpeakable(const inflection::dialog::NumberConcept& num, const std::u16string& style,
    const std::u16string& variant, bool hasVariant, UErrorCode& errorCode)
{
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

// Operand -> PronounConcept. String = a pronoun seed (PronounConcept(model, seed) THROWS on a bad
// seed, so it is caught and mapped).
static std::unique_ptr<inflection::dialog::PronounConcept> pronounFromOperand(
    const Formattable& operand, const inflection::dialog::SemanticFeatureModel* formatterModel, UErrorCode& errorCode)
{
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
                const auto* pronoun
                    = inflection::exception::ClassCastException::verifyCast<const inflection::dialog::PronounConcept*>(arg->getConcept());
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

class InflectionFunctionValue : public FunctionValue {
public:
    InflectionFunctionValue(const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options,
        UnicodeString formattedVal, UnicodeString selectVal, bool selectable, UErrorCode& errorCode)
        : formatted(std::move(formattedVal))
        , selected(std::move(selectVal))
        , canSelect(selectable)
    {
        innerValue = Formattable(formatted);
        locale = context.getLocale();
        inputDir = context.getDirection();
        if (inputDir == icu::U_MF_BIDI_OPTION_LTR) {
            dir = icu::U_MF_DIRECTIONALITY_LTR;
        } else if (inputDir == icu::U_MF_BIDI_OPTION_RTL) {
            dir = icu::U_MF_DIRECTIONALITY_RTL;
        } else if (inputDir == icu::U_MF_BIDI_OPTION_AUTO || inputDir == icu::U_MF_BIDI_OPTION_INHERIT) {
            dir = locale.isRightToLeft() ? icu::U_MF_DIRECTIONALITY_RTL : icu::U_MF_DIRECTIONALITY_LTR;
        } else {
            dir = icu::U_MF_DIRECTIONALITY_UNKNOWN;
        }
        opts = options.mergeOptions(operand.getResolvedOptions(), errorCode);
        functionName = context.getCalledFunctionName();
        fallback = operand.getFallback();
    }

    ~InflectionFunctionValue() override;

    UnicodeString formatToString(UErrorCode& status) const override
    {
        if (U_FAILURE(status)) {
            return { };
        }
        return formatted;
    }

    UBool isSelectable() const override { return canSelect; }

    void selectKeys(const UnicodeString* keys, int32_t keysLen, int32_t* prefs, int32_t& prefsLen, UErrorCode& status) const override
    {
        if (U_FAILURE(status)) {
            return;
        }
        if (!canSelect) {
            status = U_MF_SELECTOR_ERROR;
            return;
        }
        prefsLen = 0;
        for (int32_t i = 0; i < keysLen; i++) {
            if (selected == keys[i]) {
                prefs[prefsLen++] = i;
            }
        }
    }

private:
    UnicodeString formatted { };
    UnicodeString selected { };
    bool canSelect = false;
};

InflectionFunctionValue::~InflectionFunctionValue() = default;

static LocalPointer<FunctionValue> formattingError(UErrorCode& errorCode)
{
    errorCode = U_MF_FORMATTING_ERROR;
    return LocalPointer<FunctionValue>(nullptr);
}

static LocalPointer<FunctionValue> makeFunctionValue(const FunctionContext& context, const FunctionValue& operand,
    const FunctionOptions& options, UnicodeString formattedVal, UnicodeString selectVal, bool selectable, UErrorCode& errorCode)
{
    LocalPointer<FunctionValue> result(
        new InflectionFunctionValue(context, operand, options, std::move(formattedVal), std::move(selectVal), selectable, errorCode));
    if (!result.isValid()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

class InflectionFunction : public Function {
public:
    explicit InflectionFunction(MF2Factory::OutputMode mode)
        : mode(mode)
    {
    }
    ~InflectionFunction() override = default;
    LocalPointer<FunctionValue> call(
        const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const override;

private:
    MF2Factory::OutputMode mode;
};

Function* MF2Factory::createInflectFunction(OutputMode mode) { return new InflectionFunction(mode); }

LocalPointer<FunctionValue> InflectionFunction::call(
    const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return LocalPointer<FunctionValue>(nullptr);
    }
    if (operand.isNullOperand()) {
        return formattingError(errorCode);
    }
    const Locale& locale = context.getLocale();
    const auto* model = GetSemanticFeatureModel(locale);
    const Formattable& toFormat = operand.unwrap();

    UErrorCode localStatus = U_ZERO_ERROR;
    auto conceptObj = conceptFromOperand(toFormat, model, localStatus);
    if (conceptObj == nullptr || U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    UnicodeString inputString;
    if (toFormat.getType() == UFMT_STRING) {
        inputString = toFormat.getString(localStatus);
    }
    const auto& opt = options.getOptions();
    for (const auto& [key, valPtr] : opt) {
        if (key == UnicodeString(OPTION_TO)) {
            continue; // control option (the projection feature), not a grammeme constraint
        }
        auto constraint = conceptObj->getModel()->getFeature(key);
        if (constraint == nullptr) {
            // A bare option must name a known grammatical feature.
            return formattingError(errorCode);
        }
        conceptObj->putConstraint(*constraint, resolveConstraintValue(model, *constraint, valPtr->unwrap(), localStatus));
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
    }
    std::unique_ptr<inflection::dialog::SpeakableString> speakable;
    if (opt.contains(OPTION_TO)) {
        const UnicodeString& toName = opt.at(OPTION_TO)->unwrap().getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        auto feature = conceptObj->getModel()->getFeature(toName);
        if (feature != nullptr) {
            speakable.reset(conceptObj->getFeatureValue(*feature));
        }
        // Unknown to= feature: fall back to the uninflected form / match only catch-all
        // (intentional, not an error).
    } else {
        speakable.reset(conceptObj->toSpeakableString());
    }
    UnicodeString formatted;
    UnicodeString selected;
    if (speakable != nullptr) {
        formatted = emit(*speakable, mode);
        selected = speakable->getPrint();
    } else {
        // conceptObj returns nullptr when the constrained values are an empty set.
        // Fallback to the inputString, same as the non-string case.
        formatted = emit(inflection::dialog::SpeakableString(inputString), mode);
    }
    return makeFunctionValue(context, operand, options, std::move(formatted), std::move(selected), true, errorCode);
}

class NumeralFunction : public Function {
public:
    explicit NumeralFunction(MF2Factory::OutputMode mode)
        : mode(mode)
    {
    }
    ~NumeralFunction() override = default;
    LocalPointer<FunctionValue> call(
        const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const override;

private:
    MF2Factory::OutputMode mode;
};

Function* MF2Factory::createNumeralFunction(OutputMode mode) { return new NumeralFunction(mode); }

LocalPointer<FunctionValue> NumeralFunction::call(
    const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return LocalPointer<FunctionValue>(nullptr);
    }
    if (operand.isNullOperand()) {
        return formattingError(errorCode);
    }
    inflection::util::ULocale locale(context.getLocale().getName());
    const Formattable& toFormat = operand.unwrap();
    UErrorCode localStatus = U_ZERO_ERROR;
    std::unique_ptr<inflection::dialog::NumberConcept> num;
    if (toFormat.getType() == UFMT_DOUBLE) {
        num = std::make_unique<inflection::dialog::NumberConcept>(toFormat.getDouble(localStatus), locale, locale);
    } else {
        num = std::make_unique<inflection::dialog::NumberConcept>(toFormat.getInt64(localStatus), locale, locale);
    }
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    std::u16string style;
    std::u16string variant;
    bool hasVariant = false;
    const auto& opt = options.getOptions();
    if (opt.contains(OPTION_WITH_STYLE)) {
        const UnicodeString& v = opt.at(OPTION_WITH_STYLE)->unwrap().getString(localStatus);
        style = std::u16string(v.getBuffer(), v.length());
    }
    if (opt.contains(OPTION_WITH_VARIANT)) {
        const UnicodeString& v = opt.at(OPTION_WITH_VARIANT)->unwrap().getString(localStatus);
        variant = std::u16string(v.getBuffer(), v.length());
        hasVariant = true;
    }
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    inflection::dialog::SpeakableString result = numeralToSpeakable(*num, style, variant, hasVariant, localStatus);
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }
    UnicodeString out = emit(result, mode);
    return makeFunctionValue(context, operand, options, std::move(out), { }, false, errorCode);
}

class QuantifyFunction : public Function {
public:
    explicit QuantifyFunction(MF2Factory::OutputMode mode)
        : mode(mode)
    {
    }
    ~QuantifyFunction() override = default;
    LocalPointer<FunctionValue> call(
        const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const override;

private:
    MF2Factory::OutputMode mode;
};

Function* MF2Factory::createQuantifyFunction(OutputMode mode) { return new QuantifyFunction(mode); }

LocalPointer<FunctionValue> QuantifyFunction::call(
    const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return LocalPointer<FunctionValue>(nullptr);
    }
    if (operand.isNullOperand()) {
        return formattingError(errorCode);
    }
    const Locale& icuLoc = context.getLocale();
    const auto* model = GetSemanticFeatureModel(icuLoc);
    const auto* ccf = GetCommonConceptFactory(icuLoc);
    inflection::util::ULocale locale(icuLoc.getName());

    UErrorCode localStatus = U_ZERO_ERROR;
    std::unique_ptr<inflection::dialog::SemanticFeatureConceptBase> unit = conceptFromOperand(operand.unwrap(), model, localStatus);
    if (unit == nullptr || U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    const auto& opt = options.getOptions();
    if (!opt.contains(OPTION_WITH_VALUE)) {
        return formattingError(errorCode);
    }
    if (opt.contains(OPTION_WITH_STYLE) || opt.contains(OPTION_WITH_VARIANT)) {
        // reserved, not implemented, but perhaps it will be used in the future.
        return formattingError(errorCode);
    }

    const Formattable& valueF = opt.at(OPTION_WITH_VALUE)->unwrap();
    std::unique_ptr<inflection::dialog::NumberConcept> num;
    if (valueF.getType() == UFMT_DOUBLE) {
        num = std::make_unique<inflection::dialog::NumberConcept>(valueF.getDouble(localStatus), locale, locale);
    } else {
        // Integer-valued args (UFMT_INT64/UFMT_LONG) take this branch; only genuine doubles take the branch above.
        num = std::make_unique<inflection::dialog::NumberConcept>(valueF.getInt64(localStatus), locale, locale);
    }
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    // Control options (exact allow-list, NOT a with-prefix test: some real semantic features
    // are with-prefixed). withValue consumed above; withStyle/withVariant rejected above.
    for (const auto& entry : opt) {
        const UnicodeString& key = entry.first;
        if (key == UnicodeString(OPTION_WITH_VALUE) || key == UnicodeString(OPTION_WITH_STYLE)
            || key == UnicodeString(OPTION_WITH_VARIANT)) {
            continue;
        }
        std::u16string name(key.getBuffer(), key.length());
        const auto* feature = unit->getModel()->getFeature(name);
        if (feature == nullptr) {
            return formattingError(errorCode); // unknown feature (mirror FormatTest strictness)
        }
        const UnicodeString& v = entry.second->unwrap().getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        unit->putConstraint(*feature, std::u16string(v.getBuffer(), v.length()));
    }

    std::unique_ptr<inflection::dialog::SpeakableString> result(ccf->quantify(*num, unit.get()));
    if (result == nullptr) {
        return formattingError(errorCode);
    }
    UnicodeString out = emit(*result, mode);
    return makeFunctionValue(context, operand, options, std::move(out), { }, false, errorCode);
}

class ListFunction : public Function {
public:
    explicit ListFunction(MF2Factory::OutputMode mode)
        : mode(mode)
    {
    }
    ~ListFunction() override = default;
    LocalPointer<FunctionValue> call(
        const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const override;

private:
    MF2Factory::OutputMode mode;
};

Function* MF2Factory::createListFunction(OutputMode mode) { return new ListFunction(mode); }

LocalPointer<FunctionValue> ListFunction::call(
    const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return LocalPointer<FunctionValue>(nullptr);
    }
    if (operand.isNullOperand()) {
        return formattingError(errorCode);
    }
    const Locale& icuLoc = context.getLocale();
    const auto* model = GetSemanticFeatureModel(icuLoc);
    const auto* ccf = GetCommonConceptFactory(icuLoc);

    const Formattable& arrF = operand.unwrap();
    if (arrF.getType() != UFMT_ARRAY) {
        return formattingError(errorCode);
    }
    UErrorCode localStatus = U_ZERO_ERROR;
    int32_t count = 0;
    const Formattable* elems = arrF.getArray(count, localStatus);
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    const auto& opt = options.getOptions();

    std::vector<std::unique_ptr<inflection::dialog::InflectableStringConcept>> items;
    std::vector<const inflection::dialog::SemanticFeatureConceptBase*> itemPtrs;
    for (int32_t i = 0; i < count; i++) {
        UnicodeString elemStr = elems[i].getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        items.push_back(
            std::make_unique<inflection::dialog::InflectableStringConcept>(model, inflection::dialog::SpeakableString(elemStr)));
        itemPtrs.push_back(items.back().get());
    }

    std::unique_ptr<inflection::dialog::SemanticConceptList> list;
    if (!opt.contains(OPTION_WITH_TYPE)) {
        list = std::make_unique<inflection::dialog::SemanticConceptList>(model, itemPtrs);
    } else {
        const UnicodeString& typeV = opt.at(OPTION_WITH_TYPE)->unwrap().getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        std::u16string typeVal(typeV.getBuffer(), typeV.length());
        if (typeVal == TYPE_AND) {
            list.reset(ccf->createAndList(itemPtrs));
        } else if (typeVal == TYPE_OR) {
            list.reset(ccf->createOrList(itemPtrs));
        } else {
            return formattingError(errorCode);
        }
    }
    if (list == nullptr) {
        return formattingError(errorCode);
    }

    // Apply separator-setter options.
    if (opt.contains(OPTION_WITH_BEFORE_FIRST)) {
        list->setBeforeFirst(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_BEFORE_FIRST)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_AFTER_FIRST)) {
        list->setAfterFirst(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_AFTER_FIRST)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_ITEM_DELIMITER)) {
        list->setItemDelimiter(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_DELIMITER)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_BEFORE_LAST)) {
        list->setBeforeLast(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_BEFORE_LAST)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_AFTER_LAST)) {
        list->setAfterLast(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_AFTER_LAST)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_ITEM_PREFIX)) {
        list->setItemPrefix(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_PREFIX)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_ITEM_SUFFIX)) {
        list->setItemSuffix(inflection::dialog::SpeakableString(opt.at(OPTION_WITH_ITEM_SUFFIX)->unwrap().getString(localStatus)));
    }
    if (opt.contains(OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY)) {
        const UnicodeString& v = opt.at(OPTION_WITH_AVOID_ITEM_AFFIX_REDUNDANCY)->unwrap().getString(localStatus);
        list->setAvoidItemAffixRedundancy(std::u16string(v.getBuffer(), static_cast<size_t>(v.length())) == VALUE_TRUE);
    }
    if (U_FAILURE(localStatus)) {
        return formattingError(errorCode);
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
        if (inflection::util::ArrayUtils::contains<LIST_CONTROL>(nameView)) {
            continue; // control options (exact allow-list)
        }
        const auto* feature = model->getFeature(nameView);
        if (feature == nullptr) {
            return formattingError(errorCode); // unknown feature (mirror FormatTest/:quantify strictness)
        }
        const UnicodeString& v = entry.second->unwrap().getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        list->putConstraint(*feature, std::u16string(v.getBuffer(), v.length()));
    }

    std::unique_ptr<inflection::dialog::SpeakableString> result(list->toSpeakableString());
    if (result == nullptr) {
        return formattingError(errorCode);
    }
    UnicodeString out = emit(*result, mode);
    return makeFunctionValue(context, operand, options, std::move(out), { }, false, errorCode);
}

class PronounFunction : public Function {
public:
    explicit PronounFunction(MF2Factory::OutputMode mode)
        : mode(mode)
    {
    }
    ~PronounFunction() override = default;
    LocalPointer<FunctionValue> call(
        const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const override;

private:
    MF2Factory::OutputMode mode;
};

Function* MF2Factory::createPronounFunction(OutputMode mode) { return new PronounFunction(mode); }

LocalPointer<FunctionValue> PronounFunction::call(
    const FunctionContext& context, const FunctionValue& operand, const FunctionOptions& options, UErrorCode& errorCode) const
{
    if (U_FAILURE(errorCode)) {
        return LocalPointer<FunctionValue>(nullptr);
    }
    if (operand.isNullOperand()) {
        return formattingError(errorCode);
    }
    const auto* model = GetSemanticFeatureModel(context.getLocale());
    UErrorCode localStatus = U_ZERO_ERROR;
    auto pronoun = pronounFromOperand(operand.unwrap(), model, localStatus);
    if (pronoun == nullptr || U_FAILURE(localStatus)) {
        return formattingError(errorCode);
    }

    const auto& opt = options.getOptions();
    if (opt.contains(OPTION_TO) && opt.contains(OPTION_WITH_REFERENT)) {
        return formattingError(errorCode);
    }
    for (const auto& [key, valPtr] : opt) {
        if (key == UnicodeString(OPTION_TO) || key == UnicodeString(OPTION_WITH_REFERENT)) {
            continue;
        }
        auto feature = pronoun->getModel()->getFeature(key);
        if (feature == nullptr) {
            return formattingError(errorCode);
        }
        pronoun->putConstraint(*feature, resolveConstraintValue(model, *feature, valPtr->unwrap(), localStatus));
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
    }

    std::unique_ptr<inflection::dialog::SpeakableString> speakable;
    if (opt.contains(OPTION_TO)) {
        const UnicodeString& toName = opt.at(OPTION_TO)->unwrap().getString(localStatus);
        if (U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        auto feature = pronoun->getModel()->getFeature(toName);
        if (feature != nullptr) {
            speakable.reset(pronoun->getFeatureValue(*feature));
        }
        // Unknown to= feature: leave null -> fallback (intentional, not an error).
    } else if (opt.contains(OPTION_WITH_REFERENT)) {
        auto referent = conceptFromOperand(opt.at(OPTION_WITH_REFERENT)->unwrap(), model, localStatus);
        if (referent == nullptr || U_FAILURE(localStatus)) {
            return formattingError(errorCode);
        }
        speakable.reset(pronoun->toSpeakableString(*referent));
    } else {
        speakable.reset(pronoun->toSpeakableString());
    }

    UnicodeString formatted;
    UnicodeString selected;
    if (speakable != nullptr) {
        formatted = emit(*speakable, mode);
        selected = speakable->getPrint();
    }
    return makeFunctionValue(context, operand, options, std::move(formatted), std::move(selected), true, errorCode);
}

}  // namespace inflection::message2
