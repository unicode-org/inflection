/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/NullPointerException.hpp>

template<typename T>
static T* npc(T* t)
{
    if (!t) throw inflection::exception::NullPointerException();
    return t;
}
