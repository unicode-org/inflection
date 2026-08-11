/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryProvider.hpp>
#include <inflection/dialog/CommonConceptFactory.hpp>
#include <inflection/util/ULocale.hpp>
#include <map>
#include <mutex>

/**
 * @brief A provider of CommonConceptFactory by specifying a locale
 */
class INFLECTION_CLASS_API inflection::dialog::LocalizedCommonConceptFactoryProvider
    : public virtual CommonConceptFactoryProvider
{
public:
    /// @cond
    typedef ::inflection::dialog::CommonConceptFactoryProvider super;
    /// @endcond

private:
    ::std::map<::inflection::util::ULocale, ::inflection::dialog::CommonConceptFactory*> commonConceptFactoryCache {  };
    ::std::mutex mutex {  };

public:
    /**
     * Get a CommonConceptFactory for the requested locale.
     */
    const ::inflection::dialog::CommonConceptFactory* getCommonConceptFactory(const ::inflection::util::ULocale& locale) override;
    /**
     * Get the default LocalizedCommonConceptFactoryProvider.
     */
    static LocalizedCommonConceptFactoryProvider* getDefaultCommonConceptFactoryProvider();

    /**
     * Default constructor
     */
    LocalizedCommonConceptFactoryProvider();
    /**
     * Destructor
     */
    ~LocalizedCommonConceptFactoryProvider() override;
};
