/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

/**
 * Version of Morphuntion
 */
#include <morphuntion/version.h>

#if !MORPHUNTION_STATIC
#define MORPHUNTION_EXPORT_API __attribute__((visibility("default")))
#else
#define MORPHUNTION_EXPORT_API
#endif

/**
 * The header and symbols of the class are exported from the library.
 */
#define MORPHUNTION_CLASS_API MORPHUNTION_EXPORT_API
/**
 * The symbols of the class are exported from the library, but the header is not exported.
 */
#define MORPHUNTION_INTERNAL_API MORPHUNTION_EXPORT_API

/**
 * The C symbol is exported from the library.
 */
#ifdef __cplusplus
#define MORPHUNTION_CAPI extern "C" MORPHUNTION_EXPORT_API
#define MORPHUNTION_CTYPE(name) extern "C" {typedef struct name name;}
#define MORPHUNTION_CBEGIN extern "C" {
#define MORPHUNTION_CEND }
#else
#define MORPHUNTION_CAPI extern MORPHUNTION_EXPORT_API
#define MORPHUNTION_CTYPE(name) typedef struct name name;
#define MORPHUNTION_CBEGIN
#define MORPHUNTION_CEND
#include <stdint.h>

#if (defined(__clang__) && defined(__CHAR16_TYPE__) && !defined(char16_t))
typedef __CHAR16_TYPE__ char16_t;
#else
// This is typically gcc
#include <uchar.h>
#endif

#endif
