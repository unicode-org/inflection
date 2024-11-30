/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/NullPointerException.hpp>

template<typename T>
static T* npc(T* t)
{
    if (!t) throw morphuntion::exception::NullPointerException();
    return t;
}
