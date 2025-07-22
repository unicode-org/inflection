/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/util/Finally.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <unicode/ulistformatter.h>
#include <unicode/ulocdata.h>
#include <memory>

namespace inflection::dialog {

CommonConceptFactoryImpl::CommonConceptFactoryImpl(const ::inflection::util::ULocale& language)
    : CommonConceptFactoryImpl(language, ::std::u16string(), {})
{
}

static constexpr int32_t DEFAULT_QUOTE_SIZE = 4;
static ::std::u16string getQuote(ULocaleData* localeData, ULocaleDataDelimiterType type) {
    ::std::u16string result(DEFAULT_QUOTE_SIZE, 0);
    auto status = U_ZERO_ERROR;
    int32_t length = ulocdata_getDelimiter(localeData, type, (UChar *)&(result[0]), int32_t(result.length()), &status);
    ::inflection::exception::ICUException::throwOnFailure(status); // Should never happen.
    result.resize(length, 0);
    return result;
}

static constexpr int32_t DEFAULT_LIST_SIZE = 64;
static std::pair<SpeakableString, SpeakableString> getDefaultList(const ::inflection::util::ULocale& language, UListFormatterType type, bool twoItems) {
    ::std::u16string formatResult(DEFAULT_LIST_SIZE, 0);
    const char16_t* const listItems[] = { u"", u"|", u"" };
    constexpr int32_t listItemsSize = std::ssize(listItems);
    const char16_t* const list2Items[] = { u"|", u"" };
    constexpr int32_t list2ItemsSize = std::ssize(list2Items);

    UErrorCode status = U_ZERO_ERROR;
    UListFormatter* listFmt = ulistfmt_openForType(language.getName().c_str(), type, ULISTFMT_WIDTH_WIDE, &status);
    ::inflection::util::Finally finally([listFmt]() noexcept {
        ulistfmt_close(listFmt);
    });
    ::inflection::exception::ICUException::throwOnFailure(status);
    auto listSize = ulistfmt_format(listFmt, reinterpret_cast<const UChar *const *>(twoItems ? list2Items : listItems), nullptr, twoItems ? list2ItemsSize : listItemsSize,
                    (UChar *)&(formatResult[0]), int32_t(formatResult.length()), &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    formatResult.resize(listSize, 0);

    // We don't want non-breaking spaces in the result.
    std::u16string result;
    inflection::util::StringViewUtils::replace(&result, formatResult, u"\u00A0", u" ");

    auto split = result.find(u"|");
    inflection::util::Validate::isTrue(split != std::u16string::npos);

    return {SpeakableString(result.substr(0, split)),
            SpeakableString(result.substr(split + 1, result.length() - split - 1))};
}

CommonConceptFactoryImpl::CommonConceptFactoryImpl(const ::inflection::util::ULocale& language, const ::std::u16string& semanticFeatureForNumberConcept, const ::std::map<::std::u16string, ::std::u16string>& semanticFeatureToNumberConceptName)
    : super()
    , language(language)
    , semanticFeatureModel(language)
    , semanticFeatureCount(semanticFeatureModel.getFeature(u"number"))
    , semanticFeatureMeasure(semanticFeatureModel.getFeature(u"measure"))
    , semanticFeatureForNumberConcept(!semanticFeatureForNumberConcept.empty() ? semanticFeatureModel.getFeature(semanticFeatureForNumberConcept) : nullptr)
    , semanticFeatureToNumberConceptName(semanticFeatureToNumberConceptName)
    , itemsAnd(getDefaultList(language, ULISTFMT_TYPE_AND, false))
    , twoItemsAnd(getDefaultList(language, ULISTFMT_TYPE_AND, true).second)
    , itemsOr(getDefaultList(language, ULISTFMT_TYPE_OR, false))
    , twoItemsOr(getDefaultList(language, ULISTFMT_TYPE_OR, true).second)
    , plurality(language)
{
    auto status = U_ZERO_ERROR;
    auto localeData = ulocdata_open(language.getName().c_str(), &status);
    ::inflection::util::Finally finally([localeData]() noexcept {
        ulocdata_close(localeData);
    });
    ::inflection::exception::ICUException::throwOnFailure(status);
    startQuote = getQuote(localeData, ULOCDATA_QUOTATION_START);
    endQuote = getQuote(localeData, ULOCDATA_QUOTATION_END);
}

CommonConceptFactoryImpl::~CommonConceptFactoryImpl()
{
}

const SemanticFeatureModel* CommonConceptFactoryImpl::getSemanticFeatureModel() const
{
    return &semanticFeatureModel;
}

const ::inflection::util::ULocale& CommonConceptFactoryImpl::getLanguage() const
{
    return language;
}

InflectableStringConcept* CommonConceptFactoryImpl::createConcept(const SpeakableString& string) const
{
    return new InflectableStringConcept(&semanticFeatureModel, string);
}

static ::std::vector<const SemanticFeatureConceptBase*> convertToConst(const ::std::vector<SemanticFeatureConceptBase*>& values)
{
    ::std::vector<const SemanticFeatureConceptBase*> results;
    results.reserve(values.size());
    for (auto value : values) {
        if (value != nullptr) {
            results.push_back(value);
        }
    }
    return results;
}

SemanticConceptList* CommonConceptFactoryImpl::createOrList(const ::std::vector<const SemanticFeatureConceptBase*>& concepts) const
{
    if (concepts.size() == 2) {
        return npc(new SemanticConceptList(&semanticFeatureModel, concepts))->setBeforeLast(twoItemsOr);
    }
    else {
        return npc(npc(new SemanticConceptList(&semanticFeatureModel, concepts))->setItemDelimiter(itemsOr.first))->setBeforeLast(itemsOr.second);
    }
}

SemanticConceptList* CommonConceptFactoryImpl::createOrList(const ::std::vector<SemanticFeatureConceptBase*>& concepts) const
{
    return createOrList(convertToConst(concepts));
}

SemanticConceptList* CommonConceptFactoryImpl::createAndList(const ::std::vector<const SemanticFeatureConceptBase*>& concepts) const
{
    if (concepts.size() == 2) {
        return npc(new SemanticConceptList(&semanticFeatureModel, concepts))->setBeforeLast(twoItemsAnd);
    }
    else {
        return npc(npc(new SemanticConceptList(&semanticFeatureModel, concepts))->setItemDelimiter(itemsAnd.first))->setBeforeLast(itemsAnd.second);
    }
}

SemanticConceptList* CommonConceptFactoryImpl::createAndList(const ::std::vector<SemanticFeatureConceptBase*>& concepts) const
{
    return createAndList(convertToConst(concepts));
}

NumberConcept* CommonConceptFactoryImpl::number(const ::inflection::util::ULocale& regionFormatLocale, int64_t number) const
{
    return new NumberConcept(number, language, ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(language, regionFormatLocale));
}

NumberConcept* CommonConceptFactoryImpl::number(const ::inflection::util::ULocale& regionFormatLocale, double number) const
{
    return new NumberConcept(number, language, ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(language, regionFormatLocale));
}

SpeakableString CommonConceptFactoryImpl::quote(const SpeakableString& str) const
{
    if (str.speakEqualsPrint()) {
        return SpeakableString(startQuote + str.getPrint() + endQuote);
    }
    return SpeakableString(startQuote + str.getPrint() + endQuote, str.getSpeak());
}

SpeakableString CommonConceptFactoryImpl::quantifiedJoin(const SpeakableString& formattedNumber, const SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule /*countType*/) const
{
    return formattedNumber + SpeakableString(u" ") + nounPhrase;
}

SpeakableString CommonConceptFactoryImpl::quantifyType(const SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const
{
    ::std::unique_ptr<SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
        ::std::u16string type;
        if (Plurality::Rule::ONE == countType) {
            type = u"singular";
        } else if (Plurality::Rule::TWO == countType) {
            type = u"dual";
        } else {
            type = u"plural";
        }
        if (npc(semanticFeatureCount)->getBoundedValues().contains(type)) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), type);
        }
        speakableResult.reset(semanticConceptClone->toSpeakableString());
    }
    if (speakableResult == nullptr) {
        speakableResult.reset(semanticConcept.toSpeakableString());
    }
    ::std::u16string measureWord;
    if (semanticFeatureMeasure != nullptr) {
        auto measureWordValue = semanticConcept.getConstraint(*npc(semanticFeatureMeasure));
        if (measureWordValue != nullptr) {
            measureWord = *npc(measureWordValue);
        }
    }
    return quantifiedJoin(formattedNumber, *npc(speakableResult.get()), measureWord, countType);
}

SpeakableString* CommonConceptFactoryImpl::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::std::unique_ptr<SpeakableString> formattedNumber;
    if (semanticFeatureForNumberConcept != nullptr) {
        ::std::unique_ptr<SpeakableString> speakableFeatureValue(npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureForNumberConcept)));
        if (speakableFeatureValue.get() != nullptr) {
            auto rulename = semanticFeatureToNumberConceptName.find(speakableFeatureValue->getPrint());
            if (rulename != semanticFeatureToNumberConceptName.end()) {
                formattedNumber.reset(new SpeakableString(number.asSpokenWords(rulename->second)));
            }
        }
    }
    if (formattedNumber == nullptr) {
        formattedNumber.reset(number.toSpeakableString());
    }
    return quantify(number, *formattedNumber, semanticConcept);
}

SpeakableString* CommonConceptFactoryImpl::quantify(const NumberConcept& number, const SpeakableString& formattedNumber, const SemanticFeatureConceptBase* semanticConcept) const
{
    auto originalCountConstraint = (semanticFeatureCount == nullptr ? nullptr : npc(semanticConcept)->getConstraint(*npc(semanticFeatureCount)));
    auto defaultSupportedCount = plurality.getDefaultSupportedCount();
    auto selectedCount = plurality.select(number.doubleValue());
    SpeakableString* result;
    if (defaultSupportedCount.size() <= 1) {
        result = new SpeakableString(quantifyType(formattedNumber, *npc(semanticConcept), true, Plurality::Rule::OTHER));
    }
    else {
        result = new SpeakableString(quantifyType(formattedNumber, *npc(semanticConcept), false, selectedCount));
    }
    ::inflection::util::Validate::isTrue(semanticFeatureCount == nullptr || originalCountConstraint == npc(semanticConcept)->getConstraint(*npc(semanticFeatureCount)));
    return result;
}

} // namespace inflection::dialog
