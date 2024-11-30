/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/Object.hpp>

/**
 * @brief Allows creation of CommonConceptFactory by specifying a locale
 * @details Pass in a locale to get a _cached_ CommonConceptFactory.
 *
 * Morphuntion _caches_ CommonConceptFactories in order to save memory across a multithreaded environment.
 * Whenever you need a CommonConceptFactory, please use this class in order to get one.
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::CommonConceptFactoryProvider
    : public virtual ::morphuntion::Object
{
public:
    /**
     * Returns a CommonConceptFactory for the given locale.
     * @param locale The locale for the CommonConceptFactory.
     * @return The CommonConceptFactory for the specified locale.
     */
    virtual const ::morphuntion::dialog::CommonConceptFactory* getCommonConceptFactory(const ::morphuntion::util::ULocale& locale) = 0;
    /**
     * The destructor
     */
    ~CommonConceptFactoryProvider() override;
};
