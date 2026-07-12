//
//  Created by Juan Carlos Garcia on 3/22/19.
//  Copyright 2019-2024 Apple Inc. All rights reserved.
//
#include <inflection/resources/DataRegistrationService.hpp>

#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <filesystem>
#include <map>
#include <mutex>

namespace inflection::resources {

static ::std::mutex& CLASS_MUTEX() {
    static auto classMutex = new ::std::mutex();
    return *npc(classMutex);
}

static ::std::map<::std::string, ::std::string, std::less<>>* PATHS_MAP()
{
    static auto PATHS_MAP_ = new ::std::map<::std::string, ::std::string, std::less<>>();
    return PATHS_MAP_;
}

static std::string_view fallthroughLocaleString(const inflection::util::ULocale& locale)
{
    static constexpr struct {
        const char* from;
        const char* to;
    } fallthroughMap[] = {
        /*
        This list describes how Unicode inflection will register and lookup data through it's DataRegistration API.

        It is written as a deny list. Locales that are listed here will either not be altered when registered,
        or be altered to match a specific locale identifier.
        All other locales will be registered and looked up through their language code.
        */

        // It's very important to keep this in sorted order for the binary search
        {"cmn", "zh_CN"},
        {"cmn_Hant", "zh_TW"},
        {"cmn_TW", "zh_TW"},
        {"iw", "he"},
        {"no", "nb"},
        {"wuu", "zh_CN"},
        {"yue", "yue_CN"},
        {"yue_CN", "yue_CN"},
        {"yue_HK", "zh_HK"},
        {"yue_Hant", "zh_HK"},
        {"zh", "zh_TW"}, // The base tokenizer uses the zh_Hant tokenizer for the unified Chinese tokenizer
        {"zh_CN", "zh_CN"},
        {"zh_HK", "zh_HK"},
        {"zh_Hans", "zh_CN"},
        {"zh_Hans_CN", "zh_CN"},
        {"zh_TW", "zh_TW"},
    };

    // Iteratively remove components until we find a match
    for (::inflection::util::ULocale localeItr = locale; !localeItr.getLanguage().empty(); localeItr = localeItr.getFallback())
    {
        auto *entry = inflection::util::ArrayUtils::searchSorted<fallthroughMap>(localeItr.getName(),
            [](const auto& item) { return item.from; });
        if (entry != nullptr) {
            return entry->to;
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
