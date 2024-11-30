/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/CommonConceptFactory.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <map>
#include <mutex>

/**
 * @brief A provider of CommonConceptFactory by specifying a locale
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::LocalizedCommonConceptFactoryProvider
    : public virtual CommonConceptFactoryProvider
{
public:
    typedef ::morphuntion::dialog::CommonConceptFactoryProvider super;

private:
    ::std::map<::morphuntion::util::ULocale, ::morphuntion::dialog::CommonConceptFactory*> commonConceptFactoryCache {  };
    ::std::mutex mutex {  };

public:
    /**
     * Get a CommonConceptFactory for the requested locale.
     */
    const ::morphuntion::dialog::CommonConceptFactory* getCommonConceptFactory(const ::morphuntion::util::ULocale& locale) override;
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
