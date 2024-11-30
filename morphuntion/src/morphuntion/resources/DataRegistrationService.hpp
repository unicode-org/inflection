//
//  Created by Juan Carlos Garcia on 3/22/19.
//  Copyright 2019-2024 Apple Inc. All rights reserved.
//
#pragma once

#include <morphuntion/resources/fwd.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <string>
#include <map>

/**
 * @brief Provides registration capabilities for registering data for Morphuntion to utilize.
 * @details This class is used by Morphuntion to add paths for varying locale's data. It should be used to specify where data exists whenever necessary,
 * for example when using Morphuntion on client devices (with downloaded data), or when testing with custom data.
 */
class MORPHUNTION_CLASS_API morphuntion::resources::DataRegistrationService final
{
public:
    /**
     * Register a path on the filesystem for Morphuntion to use as the backing data for a given locale.
     * Once the path has been set, it can not be changed to any other path.
     * If you must reload the data, you must restart the process using Morphuntion and set the new path.
     * Morphuntion is not able to change the path of its caches for several reasons:
     * <ol>
     * <li>Morphuntion is only tested with the version of the data that matches the version of the code.
     * We make no guarantees that older or newer versions of the data will work with Morphuntion.</li>
     * <li>Changing the data can change the tokenization, lemmatization and inflection behavior.
     * So you will have to reindex anything using the old version of Morphuntion's data anyway.</li>
     * <li>Changing the data in the caches is not thread safe while maintaining performance.</li>
     * <li>It's hard to guarantee that all frameworks in your process using Morphuntion either directly or indirectly can adapt to the new behavior after changing the data.
     * It's really hard to continuously ensure that all dependencies can use both the older <strong>and</strong> newer behavior in the same release.
     * Frequently, a machine learned model is trained with a specific version of the data and code of Morphuntion.
     * So if you get an untested combination of versions of dependencies, you will get undefined behavior ranging between unexpected results to a crash.</li>
     * </ol>
     *
     * @param locale The locale to register the data for.
     * @param path The path on the filesystem (relative or absolute).
     * @throws IllegalArgumentException Thrown when changing the path for a locale after it has already been set.
     * This exception is not thrown if the path is already the current value or when the path is set for the first time.
     */
    static void registerDataPathForLocale(const morphuntion::util::ULocale& locale, const std::string& path);
    /**
     * Returns the filesystem path that was previously registered for a given locale.
     *
     * @param locale The locale.
     * @return The path exactly as previously registered.
     */
    static std::string getDataPathForLocale(const morphuntion::util::ULocale& locale);
};
