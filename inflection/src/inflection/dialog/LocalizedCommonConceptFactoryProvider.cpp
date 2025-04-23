/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>

#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <inflection/dialog/language/ArCommonConceptFactory.hpp>
#include <inflection/dialog/language/DaCommonConceptFactory.hpp>
#include <inflection/dialog/language/DeCommonConceptFactory.hpp>
#include <inflection/dialog/language/EnCommonConceptFactory.hpp>
#include <inflection/dialog/language/EsCommonConceptFactory.hpp>
#include <inflection/dialog/language/FrCommonConceptFactory.hpp>
#include <inflection/dialog/language/HeCommonConceptFactory.hpp>
#include <inflection/dialog/language/HiCommonConceptFactory.hpp>
#include <inflection/dialog/language/ItCommonConceptFactory.hpp>
#include <inflection/dialog/language/JaCommonConceptFactory.hpp>
#include <inflection/dialog/language/KoCommonConceptFactory.hpp>
#include <inflection/dialog/language/MsCommonConceptFactory.hpp>
#include <inflection/dialog/language/NbCommonConceptFactory.hpp>
#include <inflection/dialog/language/NlCommonConceptFactory.hpp>
#include <inflection/dialog/language/PtCommonConceptFactory.hpp>
#include <inflection/dialog/language/RuCommonConceptFactory.hpp>
#include <inflection/dialog/language/SrCommonConceptFactory.hpp>
#include <inflection/dialog/language/SvCommonConceptFactory.hpp>
#include <inflection/dialog/language/ThCommonConceptFactory.hpp>
#include <inflection/dialog/language/TrCommonConceptFactory.hpp>
#include <inflection/dialog/language/ViCommonConceptFactory.hpp>
#include <inflection/dialog/language/YueCommonConceptFactory.hpp>
#include <inflection/dialog/language/ZhCommonConceptFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <cstring>
#include <mutex>
#include <numeric>

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

template<typename T>
static CommonConceptFactory* constructor(const ::inflection::util::ULocale& locale) {
    return new T(locale);
}

static CommonConceptFactory* constructCommonConceptFactory(const ::inflection::util::ULocale& locale)
{
    static const struct {
        const char language[4];
        CommonConceptFactory* (*construct)(const ::inflection::util::ULocale& locale);
    }
    COMMON_CONCEPT_FACTORY_CONSTRUCTORS[] = {
        // It's very important to keep this in sorted order for the binary search
        {"ar", &constructor<language::ArCommonConceptFactory>},
        {"cmn", &constructor<language::ZhCommonConceptFactory>}, // alias
        {"da", &constructor<language::DaCommonConceptFactory>},
        {"de", &constructor<language::DeCommonConceptFactory>},
        {"en", &constructor<language::EnCommonConceptFactory>},
        {"es", &constructor<language::EsCommonConceptFactory>},
        {"fr", &constructor<language::FrCommonConceptFactory>},
        {"he", &constructor<language::HeCommonConceptFactory>},
        {"hi", &constructor<language::HiCommonConceptFactory>},
        {"id", &constructor<language::MsCommonConceptFactory>}, // The words are the same for what we currently care about.
        {"it", &constructor<language::ItCommonConceptFactory>},
        {"iw", &constructor<language::HeCommonConceptFactory>}, // alias
        {"ja", &constructor<language::JaCommonConceptFactory>},
        {"ko", &constructor<language::KoCommonConceptFactory>},
        {"ms", &constructor<language::MsCommonConceptFactory>},
        {"nb", &constructor<language::NbCommonConceptFactory>},
        {"nl", &constructor<language::NlCommonConceptFactory>},
        {"pt", &constructor<language::PtCommonConceptFactory>},
        {"ru", &constructor<language::RuCommonConceptFactory>},
        {"sr", &constructor<language::SrCommonConceptFactory>},
        {"sv", &constructor<language::SvCommonConceptFactory>},
        {"th", &constructor<language::ThCommonConceptFactory>},
        {"tr", &constructor<language::TrCommonConceptFactory>},
        {"vi", &constructor<language::ViCommonConceptFactory>},
        {"wuu", &constructor<language::ZhCommonConceptFactory>},
        {"yue", &constructor<language::YueCommonConceptFactory>},
        {"zh", &constructor<language::ZhCommonConceptFactory>},
    };
    if (inflection::util::LocaleUtils::HONGKONG_CHINESE() == locale) {
        return new language::YueCommonConceptFactory(locale);
    }
    auto language(locale.getLanguage());

    int32_t start = 0;
    int32_t limit = std::ssize(COMMON_CONCEPT_FACTORY_CONSTRUCTORS) - 1;
    while (start <= limit) {
        int32_t mid = std::midpoint(start, limit);
        int32_t comp = language.compare(COMMON_CONCEPT_FACTORY_CONSTRUCTORS[mid].language);
        if (comp < 0) {
            limit = mid - 1;
        }
        else if (comp > 0) {
            start = mid + 1;
        }
        else {
            if (::inflection::util::LoggerConfig::isInfoEnabled()) {
                // We check for the logging status so that we don't construct a string that is never used.
                ::inflection::util::Logger::infoComponent(COMPONENT_LOCALIZED_COMMON_CONCEPT_FACTORY_PROVIDER, u"The CommonConceptFactory for " + locale.toString() + u" is being constructed for the first time.");
            }
            return COMMON_CONCEPT_FACTORY_CONSTRUCTORS[mid].construct(locale);
        }
    }

    if (inflection::util::LocaleUtils::getSupportedLocaleMap().contains(language)) {
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
