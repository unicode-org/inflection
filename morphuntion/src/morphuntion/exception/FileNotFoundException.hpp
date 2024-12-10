/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/IOException.hpp>

/**
 * @brief Used to indicate a missing file.
 * @details This is thrown in many places, such as by factories for tokenization, analysis, and by the resource loader.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::FileNotFoundException
    : public morphuntion::exception::IOException
{
public:
    typedef ::morphuntion::exception::IOException super;


public:
    /**
     * Default constructor
     */
    FileNotFoundException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit FileNotFoundException(const std::u16string& message);
    /**
     * Destructor
     */
    ~FileNotFoundException() override;
};
