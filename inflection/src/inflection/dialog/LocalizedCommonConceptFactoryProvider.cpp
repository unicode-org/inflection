/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>

#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <inflection/dialog/language/ArCommonConceptFactory.hpp>
#include <inflection/dialog/language/CsCommonConceptFactory.hpp>
#include <inflection/dialog/language/DeCommonConceptFactory.hpp>
#include <inflection/dialog/language/EsCommonConceptFactory.hpp>
#include <inflection/dialog/language/FiCommonConceptFactory.hpp>
#include <inflection/dialog/language/FrCommonConceptFactory.hpp>
#include <inflection/dialog/language/HeCommonConceptFactory.hpp>
#include <inflection/dialog/language/ItCommonConceptFactory.hpp>
#include <inflection/dialog/language/JaCommonConceptFactory.hpp>
#include <inflection/dialog/language/KoCommonConceptFactory.hpp>
#include <inflection/dialog/language/MlCommonConceptFactory.hpp>
#include <inflection/dialog/language/PlCommonConceptFactory.hpp>
#include <inflection/dialog/language/RuCommonConceptFactory.hpp>
#include <inflection/dialog/language/SerboCroatianCommonConceptFactory.hpp>
#include <inflection/dialog/language/SkCommonConceptFactory.hpp>
#include <inflection/dialog/language/ThCommonConceptFactory.hpp>
#include <inflection/dialog/language/TrCommonConceptFactory.hpp>
#include <inflection/dialog/language/UkCommonConceptFactory.hpp>
#include <inflection/dialog/language/YueCommonConceptFactory.hpp>
#include <inflection/dialog/language/ZhCommonConceptFactory.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <mutex>

using inflection::grammar::synthesis::GrammemeConstants;
using inflection::util::LocaleUtils;
using inflection::util::Logger;
using inflection::util::LoggerConfig;
using inflection::util::ULocale;

namespace inflection::dialog {

LocalizedCommonConceptFactoryProvider::LocalizedCommonConceptFactoryProvider()
    : super()
{
}

LocalizedCommonConceptFactoryProvider::~LocalizedCommonConceptFactoryProvider()
{
    for (const auto& entry : commonConceptFactoryCache) {
        delete entry.second;
    }
}

static constexpr auto COMPONENT_LOCALIZED_COMMON_CONCEPT_FACTORY_PROVIDER = u"LocalizedCommonConceptFactoryProvider";

static CommonConceptFactory* constructor_CommonConceptFactoryMasculineFeminine(const ULocale& locale) {
    return new CommonConceptFactoryImpl(locale, GrammemeConstants::GENDER, {
        {GrammemeConstants::GENDER_MASCULINE, u"cardinal-masculine"},
        {GrammemeConstants::GENDER_FEMININE, u"cardinal-feminine"}});
}

static CommonConceptFactory* constructor_CommonConceptFactoryMasculineFeminineNeuter(const ULocale& locale) {
    return new CommonConceptFactoryImpl(locale, GrammemeConstants::GENDER, {
        {GrammemeConstants::GENDER_MASCULINE, u"cardinal-masculine"},
        {GrammemeConstants::GENDER_FEMININE, u"cardinal-feminine"},
        {GrammemeConstants::GENDER_NEUTER, u"cardinal-neuter"}});
}

static CommonConceptFactory* constructor_DaCommonConceptFactory(const ULocale& locale) {
    return new CommonConceptFactoryImpl(locale, GrammemeConstants::GENDER, {
        {GrammemeConstants::GENDER_COMMON, u"cardinal-common"},
        {GrammemeConstants::GENDER_NEUTER, u"cardinal-neuter"}});
}

static CommonConceptFactory* constructor_SvCommonConceptFactory(const ULocale& locale) {
    return new CommonConceptFactoryImpl(locale, GrammemeConstants::GENDER, {
        {GrammemeConstants::GENDER_COMMON, u"cardinal-reale"},
        {GrammemeConstants::GENDER_NEUTER, u"cardinal-neuter"}});
}

template<typename T>
static CommonConceptFactory* constructor(const ::inflection::util::ULocale& locale) {
    return new T(locale);
}

static CommonConceptFactory* constructCommonConceptFactory(const ::inflection::util::ULocale& locale)
{
    static constexpr struct {
        const char language[4];
        CommonConceptFactory* (*construct)(const ::inflection::util::ULocale& locale);
    } COMMON_CONCEPT_FACTORY_CONSTRUCTORS[] = {
        // It's very important to keep this in sorted order for the binary search
        {"ar", &constructor<language::ArCommonConceptFactory>},
        {"bg", &constructor_CommonConceptFactoryMasculineFeminineNeuter}, // TODO Fix this quantity handling so that it handles the financial and masculine-personal (masculine human) types.);
        {"ca", &constructor_CommonConceptFactoryMasculineFeminine},
        {"cmn", &constructor<language::ZhCommonConceptFactory>}, // alias
        {"cs", &constructor<language::CsCommonConceptFactory>},
        {"da", &constructor_DaCommonConceptFactory},
        {"de", &constructor<language::DeCommonConceptFactory>},
        {"el", &constructor_CommonConceptFactoryMasculineFeminineNeuter},
        {"en", &constructor<CommonConceptFactoryImpl>},
        {"es", &constructor<language::EsCommonConceptFactory>},
        {"et", &constructor<CommonConceptFactoryImpl>},
        {"fi", &constructor<language::FiCommonConceptFactory>},
        {"fr", &constructor<language::FrCommonConceptFactory>},
        {"he", &constructor<language::HeCommonConceptFactory>},
        {"hr", &constructor<language::SerboCroatianCommonConceptFactory>},
        {"is", &constructor_CommonConceptFactoryMasculineFeminineNeuter},
        {"it", &constructor<language::ItCommonConceptFactory>},
        {"iw", &constructor<language::HeCommonConceptFactory>}, // alias
        {"ja", &constructor<language::JaCommonConceptFactory>},
        {"ko", &constructor<language::KoCommonConceptFactory>},
        {"lt", &constructor_CommonConceptFactoryMasculineFeminine},
        {"ml", &constructor<language::MlCommonConceptFactory>},
        {"nb", &constructor_CommonConceptFactoryMasculineFeminineNeuter},
        {"pl", &constructor<language::PlCommonConceptFactory>},
        {"pt", &constructor_CommonConceptFactoryMasculineFeminine},
        {"ro", &constructor_CommonConceptFactoryMasculineFeminineNeuter},
        {"ru", &constructor<language::RuCommonConceptFactory>},
        {"sk", &constructor<language::SkCommonConceptFactory>},
        {"sr", &constructor<language::SerboCroatianCommonConceptFactory>},
        {"sv", &constructor_SvCommonConceptFactory},
        {"th", &constructor<language::ThCommonConceptFactory>},
        {"tr", &constructor<language::TrCommonConceptFactory>},
        {"uk", &constructor<language::UkCommonConceptFactory>},
        {"wuu", &constructor<language::ZhCommonConceptFactory>},
        {"yue", &constructor<language::YueCommonConceptFactory>},
        {"zh", &constructor<language::ZhCommonConceptFactory>},
    };
    if (locale.getLanguage() == inflection::util::LocaleConstants::CHINESE
        && locale.getScript().empty()
        && locale.getCountry() == inflection::util::LocaleConstants::REGION_HONGKONG)
    {
        return new language::YueCommonConceptFactory(locale);
    }
    std::string_view language(locale.getLanguage());

    auto *entry = inflection::util::ArrayUtils::searchSorted<COMMON_CONCEPT_FACTORY_CONSTRUCTORS>(language,
            [](const auto& item) { return item.language; });
    if (entry != nullptr) {
        if (::inflection::util::LoggerConfig::isInfoEnabled()) {
            // We check for the logging status so that we don't construct a string that is never used.
            ::inflection::util::Logger::infoComponent(COMPONENT_LOCALIZED_COMMON_CONCEPT_FACTORY_PROVIDER, u"The CommonConceptFactory for " + locale.toString() + u" is being constructed for the first time.");
        }
        return entry->construct(locale);
    }

    if (inflection::util::LocaleUtils::getSupportedLocaleMap().contains(locale.getLanguage())) {
        return new CommonConceptFactoryImpl(locale);
    }
    return nullptr;
}

const CommonConceptFactory* LocalizedCommonConceptFactoryProvider::getCommonConceptFactory(const ::inflection::util::ULocale& locale)
{
    std::lock_guard<std::mutex> guard(mutex);
    auto conceptFactoryResult = commonConceptFactoryCache.find(locale);
    if (conceptFactoryResult != commonConceptFactoryCache.end()) {
        return conceptFactoryResult->second;
    }
    CommonConceptFactory* returnResult = constructCommonConceptFactory(locale);
    if (returnResult != nullptr) {
        commonConceptFactoryCache.emplace(locale, returnResult);
        return returnResult;
    }
    throw ::inflection::exception::IllegalArgumentException(locale.toString() + u" has no known CommonConceptFactory implementation");
}

LocalizedCommonConceptFactoryProvider* LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider() {
    static auto DEFAULT_LOCALIZED_COMMON_CONCEPT_FACTORY_PROVIDER = new LocalizedCommonConceptFactoryProvider();
    return DEFAULT_LOCALIZED_COMMON_CONCEPT_FACTORY_PROVIDER;
}


} // namespace inflection::dialog
