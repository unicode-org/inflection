//
//  Created by Juan Carlos Garcia on 3/22/19.
//  Copyright 2019-2024 Apple Inc. All rights reserved.
//
#include <inflection/resources/DataRegistrationService.hpp>

#include <inflection/resources/DataResource.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <filesystem>
#include <mutex>

namespace inflection::resources {

static ::std::mutex& CLASS_MUTEX() {
    static auto classMutex = new ::std::mutex();
    return *npc(classMutex);
}

static ::std::map<std::string, std::string, std::less<>>* initLOCALE_FALLTHROUGH_BLACKLIST()
{
    auto result = new std::map<std::string, std::string, std::less<>>();
    auto mappings = inflection::resources::DataResource::getProperties(u"/org/unicode/inflection/locale/registration-locale-list.properties");
    for (auto const & [from, to] : mappings) {
        // Normalize the values as necessary.
        inflection::util::ULocale key(inflection::util::StringViewUtils::to_string(from));
        inflection::util::ULocale value(inflection::util::StringViewUtils::to_string(to));
        npc(result)->emplace(key.getName(), value.getName());
    }
    return result;
}

static const std::map<std::string, std::string, std::less<>>* LOCALE_FALLTHROUGH_BLACKLIST()
{
    static auto LOCALE_FALLTHROUGH_BLACKLIST_ = initLOCALE_FALLTHROUGH_BLACKLIST();
    return LOCALE_FALLTHROUGH_BLACKLIST_;
}

static ::std::map<::std::string, ::std::string, std::less<>>* PATHS_MAP()
{
    static auto PATHS_MAP_ = new ::std::map<::std::string, ::std::string, std::less<>>();
    return PATHS_MAP_;
}

static std::string_view fallthroughLocaleString(const inflection::util::ULocale& locale)
{
    auto fallthroughMap = LOCALE_FALLTHROUGH_BLACKLIST();

    // Iteratively remove components until we find a match
    for (::inflection::util::ULocale localeItr = locale; !localeItr.getLanguage().empty(); localeItr = localeItr.getFallback())
    {
        auto itr = fallthroughMap->find(localeItr.getName());
        if (itr != fallthroughMap->end())
        {
            return itr->second;
        }
    }

    return locale.getLanguage();
}

void DataRegistrationService::registerDataPathForLocale(const inflection::util::ULocale& locale, const std::string& path)
{
    auto language(fallthroughLocaleString(locale));
    auto normalizedPath(path);
    std::string_view fileURLPrefix("file://");
    if (normalizedPath.compare(0, fileURLPrefix.size(), fileURLPrefix) == 0) {
        normalizedPath.erase(0, fileURLPrefix.length());
    }
    while (!normalizedPath.empty() && normalizedPath.back() == '/') {
        normalizedPath.pop_back();
    }
    normalizedPath = std::filesystem::weakly_canonical(std::filesystem::path(normalizedPath)).make_preferred().string();

    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    auto& pathsMap = *npc(PATHS_MAP());
    auto existingPath(pathsMap.find(language));
    bool noExistingPathConflict = existingPath == pathsMap.end() || (existingPath != pathsMap.end() && existingPath->second == normalizedPath);
    inflection::util::Validate::isTrue(noExistingPathConflict, noExistingPathConflict ? u"" : (std::u16string(u"Reloading of assets with a different path is not supported (\"") + inflection::util::StringViewUtils::to_u16string(existingPath->second) + u"\" -> \"" + inflection::util::StringViewUtils::to_u16string(path) + u"\")."));
    pathsMap.emplace(language, normalizedPath);
}

std::string DataRegistrationService::getDataPathForLocale(const inflection::util::ULocale& locale)
{
    auto language(fallthroughLocaleString(locale));
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    auto& pathsMap = *npc(PATHS_MAP());
    auto iterator = pathsMap.find(language);
    if (iterator != pathsMap.end()) {
        return iterator->second;
    }
    return ::std::string();
}

} // namespace inflection::resources
