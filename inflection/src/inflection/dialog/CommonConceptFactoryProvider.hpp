/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>

/**
 * @brief Allows creation of CommonConceptFactory by specifying a locale
 * @details Pass in a locale to get a _cached_ CommonConceptFactory.
 *
 * Inflection _caches_ CommonConceptFactories in order to save memory across a multithreaded environment.
 * Whenever you need a CommonConceptFactory, please use this class in order to get one.
 */
class INFLECTION_CLASS_API inflection::dialog::CommonConceptFactoryProvider
    : public virtual ::inflection::Object
{
public:
    /**
     * Returns a CommonConceptFactory for the given locale.
     * @param locale The locale for the CommonConceptFactory.
     * @return The CommonConceptFactory for the specified locale.
     */
    virtual const ::inflection::dialog::CommonConceptFactory* getCommonConceptFactory(const ::inflection::util::ULocale& locale) = 0;
    /**
     * The destructor
     */
    ~CommonConceptFactoryProvider() override;
};
