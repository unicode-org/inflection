/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/Plurality.hpp>

#include <inflection/exception/ICUException.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <unicode/uenum.h>

namespace inflection::dialog {

Plurality::Plurality(const ::inflection::util::ULocale& locale)
    : super()
{
    auto status = U_ZERO_ERROR;
    this->pluralRules = uplrules_open(locale.getName().c_str(), &status);
    ::inflection::exception::ICUException::throwOnFailure(status);

    auto keywords = uplrules_getKeywords(pluralRules, &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    const UChar* pluralRuleString = nullptr;
    while ((pluralRuleString = uenum_unext(keywords, nullptr, &status)) != nullptr) {
        defaultSupportedCount.emplace(convertKeywordToCount((const char16_t*)pluralRuleString));
    }
    uenum_close(keywords);
    ::inflection::exception::ICUException::throwOnFailure(status);
}

Plurality::~Plurality()
{
    uplrules_close(this->pluralRules);
}

static constexpr int32_t KEYWORD_BUFFER_SIZE = 6;

Plurality::Rule Plurality::convertKeywordToCount(std::u16string_view keyword) const
{
    static constexpr struct {
        const char16_t* keyword;
        Plurality::Rule enumValue;
    } KEYWORD_NAME_MAP[] = {
        {u"few", Plurality::Rule::FEW},
        {u"many", Plurality::Rule::MANY},
        {u"one", Plurality::Rule::ONE},
        {u"other", Plurality::Rule::OTHER},
        {u"two", Plurality::Rule::TWO},
        {u"zero", Plurality::Rule::ZERO},
    };
    auto *result = inflection::util::ArrayUtils::searchSorted<KEYWORD_NAME_MAP>(keyword,
        [](const auto& item) { return item.keyword; });
    if (result != nullptr) {
        return result->enumValue;
    }
    return Plurality::Rule::OTHER;
}

const ::std::set<Plurality::Rule>& Plurality::getDefaultSupportedCount() const
{
    return defaultSupportedCount;
}

Plurality::Rule Plurality::select(double number) const
{
    auto status = U_ZERO_ERROR;
    UChar keywordBuffer[KEYWORD_BUFFER_SIZE];
    auto size = uplrules_select(pluralRules, number, keywordBuffer, KEYWORD_BUFFER_SIZE, &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return convertKeywordToCount(std::u16string_view((const char16_t*)keywordBuffer, size));
}

} // namespace inflection::dialog
