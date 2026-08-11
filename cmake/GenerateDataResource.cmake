#
# Copyright 2026 Unicode Incorporated and others. All rights reserved.
#

file(REAL_PATH "${RESOURCES}" RESOURCES_ABSOLUTE)

file(GLOB_RECURSE RESOURCE_PROPS ${RESOURCES_ABSOLUTE}/org/[^.]*.properties)
file(GLOB_RECURSE RESOURCE_STRINGS ${RESOURCES_ABSOLUTE}/org/[^.]*)
foreach (FILTER_REGEX IN ITEMS "\\.(properties|dtd|xml|lst)$" "tokenizer\\.dictionary$" "transliteration_map_.+\\.csv$" "suffix_.+\\.csv$")
    list(FILTER RESOURCE_STRINGS EXCLUDE REGEX ${FILTER_REGEX})
endforeach ()

foreach (RESOURCE_PROP IN LISTS RESOURCE_PROPS)
    string(REGEX REPLACE "^${RESOURCES_ABSOLUTE}/(.*)\\.properties$" "#include <\\1.hpp>" RESOURCE_PROP_INCLUDE "${RESOURCE_PROP}")
    string(APPEND RESOURCE_PROP_INCLUDES "${RESOURCE_PROP_INCLUDE}\n")

    string(REGEX REPLACE "^${RESOURCES_ABSOLUTE}/(.*)\\.properties$" "\\1" RESOURCE_PROP_ITEM "${RESOURCE_PROP}")
    string(REGEX REPLACE "/" "::" RESOURCE_PROP_SCOPED "${RESOURCE_PROP_ITEM}")
    string(REGEX REPLACE "-" "_" RESOURCE_PROP_SCOPED "${RESOURCE_PROP_SCOPED}")
    string(REGEX REPLACE "\\." "_" RESOURCE_PROP_SCOPED "${RESOURCE_PROP_SCOPED}")
    set(PROPERTIES_ENTRY "        {u\"/${RESOURCE_PROP_ITEM}.properties\", ${RESOURCE_PROP_SCOPED}::getContent},\n")
    string(APPEND PROPERTIES_ENTRIES "${PROPERTIES_ENTRY}")
endforeach()

foreach (RESOURCE_STRING IN LISTS RESOURCE_STRINGS)
    string(REGEX REPLACE "^${RESOURCES_ABSOLUTE}/(.*)$" "#include <\\1.hpp>" RESOURCE_STR_INCLUDE "${RESOURCE_STRING}")
    string(APPEND RESOURCE_STR_INCLUDES "${RESOURCE_STR_INCLUDE}\n")

    string(REGEX REPLACE "^${RESOURCES_ABSOLUTE}/(.*)$" "\\1" RESOURCE_STR_ITEM "${RESOURCE_STRING}")
    string(REGEX REPLACE "/" "::" RESOURCE_STR_SCOPED "${RESOURCE_STR_ITEM}")
    string(REGEX REPLACE "-" "_" RESOURCE_STR_SCOPED "${RESOURCE_STR_SCOPED}")
    string(REGEX REPLACE "\\." "_" RESOURCE_STR_SCOPED "${RESOURCE_STR_SCOPED}")
    set(STR_ENTRY "        {u\"/${RESOURCE_STR_ITEM}\", ${RESOURCE_STR_SCOPED}::content},\n")
    string(APPEND MAP_ENTRIES "${STR_ENTRY}")
endforeach()

get_filename_component(DATA_RESOURCE_DIR ${DATA_RESOURCE_PATH} DIRECTORY)
file(MAKE_DIRECTORY ${DATA_RESOURCE_DIR})

file(WRITE "${DATA_RESOURCE_PATH}" "#include <inflection/resources/DataResource.hpp>
${RESOURCE_PROP_INCLUDES}${RESOURCE_STR_INCLUDES}#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/util/ArrayUtils.hpp>
::std::map<::std::u16string_view, const char16_t*> inflection::resources::DataResource::getProperties(::std::u16string_view resourceName, bool returnEmptyOnFailure) {
    static constexpr struct {
        const char16_t* key;
        std::map<std::u16string_view, const char16_t*> (*properties)();
    } resources[] = {
${PROPERTIES_ENTRIES}    };
    auto *entry = inflection::util::ArrayUtils::searchSorted<resources>(resourceName,
        [](const auto& item) { return item.key; });
    if (entry == nullptr) {
        if (returnEmptyOnFailure) {
            return {};
        }
        throw ::inflection::exception::FileNotFoundException(::std::u16string(resourceName));
    }

    return entry->properties();
}
const char16_t *inflection::resources::DataResource::getString(::std::u16string_view resourceName, bool returnNullOnFailure) {
    static constexpr struct {
        const char16_t* key;
        const char16_t* value;
    } resources[] = {
${MAP_ENTRIES}    };
    auto *entry = inflection::util::ArrayUtils::searchSorted<resources>(resourceName,
        [](const auto& item) { return item.key; });
    if (entry == nullptr) {
        if (returnNullOnFailure) {
            return nullptr;
        }
        throw ::inflection::exception::FileNotFoundException(::std::u16string(resourceName));
    }

    return entry->value;
}
")
