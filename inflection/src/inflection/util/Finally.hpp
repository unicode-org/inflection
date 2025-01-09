/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <type_traits>

template <typename Function>
class inflection::util::Finally final
{
public:
    explicit Finally(Function f) : f(f) {
        static_assert(std::is_nothrow_invocable<Function>(), "Function can not be throwable because a secondary thrown exception during stack unwinding will terminate the program");
    }
    ~Finally() noexcept { f(); }

private:
    Finally(const Finally&) = delete;
    Finally(Finally &&x) = delete;
    Finally& operator=(const Finally&) = delete;

    Function f;
};
